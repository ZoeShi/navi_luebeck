//
// Created by l on 08.09.18.
//
#include <clocale>
#include <iostream>
#include <gtkmm/application.h>
#include "dijkstra.h"
#include "main.h"

void Main::fill_store(Glib::RefPtr<Gtk::ListStore> store) {
    for (auto& pair : nodes) {
        auto &node = pair.second;
        auto row = *(store->append());
        row[name_column] = node.name;
        row[node_column] = &node;
    }
}

bool Main::on_draw(const Cairo::RefPtr<Cairo::Context>& context) {
    auto allocation = drawing_area.get_allocation();
    width = allocation.get_width();
    height = allocation.get_height();

    calculate_route();
    fill_route_tree_model();

    draw_edges(context);
    draw_nodes(context);
    draw_node_names(context);
    return true;
}

void Main::fill_route_tree_model () {
    route_tree_model->clear ();
    if (route.empty()) {
        return;
    }
    int prior_id = *route.begin();
    int distance = 0;
    int i = 1;
    for (int id : route) {
        auto row = *(route_tree_model->append());
        int length = 0;
        auto &node = nodes[id];
        if (prior_id != id) {
            auto &prior_node = nodes[prior_id];
            for (auto &edge : prior_node.edges) {
                if (edge.to == id) {
                    distance += edge.distance;
                    length = edge.distance;
                }
            }
        }
        row[route_node_name_column] = std::to_string(i) + " - " + node.name;
        row[route_node_distance_column] = distance;
        row[route_node_length_column] = length;
        prior_id = id;
        i++;
    }
}

void Main::calculate_route() {
    Node* from_node = nullptr;
    Node* to_node = nullptr;

    route.clear();

    get_selected_nodes(from_node, to_node);

    if (!from_node || !to_node) {
        return;
    }

    auto from_it = find_if (begin(nodes), end(nodes), [from_node] (auto &pair) {
        return &pair.second == from_node;
    });

    auto to_it = find_if (begin(nodes), end(nodes), [to_node] (auto &pair) {
        return &pair.second == to_node;
    });

    if ((from_it == end(nodes)) || (to_it == end(nodes))) {
        return;
    }

    int from_id = from_it->first;
    int to_id = to_it->first;

    Dijkstra d (nodes, from_id);
    route = d.get_route (to_id);
}

void Main::get_selected_nodes(Node *&from_node, Node *&to_node) const {
    auto from_it1 = from_combo_box.get_active();
    if (from_it1) {
        Gtk::TreeModel::Row row = *from_it1;
        from_node = row[node_column];
    }

    auto to_it1 = to_combo_box.get_active();
    if (to_it1) {
        Gtk::TreeModel::Row row = *to_it1;
        to_node = row[node_column];
    }
}

void Main::draw_objects (const Cairo::RefPtr<Cairo::Context> &context, DrawObjectFunc func) {
    for (auto &pair : nodes) {
        auto source_id = pair.first;
        auto &source_node = pair.second;
        auto source_pos = route_position(source_id);
        func (context, source_node, source_pos);
    }
}

void Main::draw_node_names (const Cairo::RefPtr<Cairo::Context> &context) {
    draw_objects (context, [this] (auto &context, auto &node, auto &node_pos) {
        draw_node_name(context, node, node_pos);
    });
}

void Main::draw_nodes(const Cairo::RefPtr<Cairo::Context> &context) {
    draw_objects (context, [this] (auto &context, auto &node, auto &node_pos) {
        draw_node(context, node, static_cast<bool> (node_pos));
    });
}

void Main::draw_edges(const Cairo::RefPtr<Cairo::Context> &context) {
    draw_objects (context, [this] (auto &context, auto &source_node, auto &source_pos) {
        for (auto &edge : source_node.edges) {
            auto target_id = edge.to;
            auto &target_node = nodes[target_id];
            auto target_pos = route_position (target_id);

            draw_edge(context, source_node, target_node, source_pos && target_pos);
        }
    });
}

std::optional<int> Main::route_position (int id) const {
    auto it = std::find (std::begin(route), std::end(route), id);
    if (it == std::end (route)) {
        return std::nullopt;
    }
    return 1 + std::distance (std::begin (route), it);
}

void Main::set_color (const Cairo::RefPtr<Cairo::Context> &context, ObjectKind kind, bool on_route) {
    Color black = {0, 0, 0};
    Color white = {1, 1, 1};

    Color color {0};
    if (on_route) {
        color = white;
    }
    else {
        color = black;
    }

    context->set_source_rgb(color.red, color.green, color.blue);
}

void Main::draw (const Cairo::RefPtr<Cairo::Context> &context, const DrawingFunc &func, ObjectKind kind, bool on_route) {
    context->save ();
    set_color (context, kind, on_route);
    func (context);
    context->stroke ();
    context->restore ();
}

void Main::move_to (const Cairo::RefPtr<Cairo::Context> &context, const Node& node) {
    Point point = get_xy_from_coords (node.coordinates);
    context->move_to (point.x, point.y);
}

void Main::line_to (const Cairo::RefPtr<Cairo::Context> &context, const Node& node) {
    Point point = get_xy_from_coords (node.coordinates);
    context->line_to(point.x, point.y);
}

void Main::draw_edge(const Cairo::RefPtr<Cairo::Context> &context, const Node &source_node,
               const Node &target_node, bool on_route) {
    draw (context, [this, &source_node, &target_node] (auto context) {
        move_to (context, source_node);
        line_to (context, target_node);
    }, ObjectKind::Edge, on_route);
}

void Main::draw_node(const Cairo::RefPtr<Cairo::Context> &context, const Node &node,
        bool on_route) {
    draw (context, [this, &node] (auto context) {
        Point pt = get_xy_from_coords(node.coordinates);
        context->arc (pt.x, pt.y, 5, 0.0, 2 * M_PI);
        context->fill_preserve();
    }, ObjectKind::Node, on_route);
}

void Main::draw_node_name(const Cairo::RefPtr<Cairo::Context> &context, const Node &node,
        const std::optional<int> &route_pos) {
    if (!route_pos) {
        return;
    }
    draw (context, [this, &node, &route_pos] (auto context) {
        Point pt = get_xy_from_coords(node.coordinates);
        context->move_to (pt.x + 10, pt.y - 5);
        auto layout = drawing_area.create_pango_layout(std::to_string (*route_pos));
        layout->show_in_cairo_context(context);
    }, ObjectKind::NodeName, true);
}

// Get relative coord from a latitude or longitude value inside the [min,max] interval
// The result will be in the [0,1] interval
double Main::get_relative_coord (double value, double min_value, double max_value) {
    return (value - min_value) / (max_value - min_value);
}

// Translate relative coord using length of axis, zoom level and shift value to an absolute position in pixels
int Main::translate_coord (double shift, double value, double length, double zoom) const {
    return static_cast<int> ((shift + value) * length * zoom);
}

// Translate earth coordinates (longitude / latitude) to our drawing widget coordinate system,
// applying some zoom and shift to make it fit better
Point Main::get_xy_from_coords (const Coordinates &coords) {
    // Zoom map
    constexpr double zoom_x = 0.8;
    constexpr double zoom_y = 0.8;

    // Shift map to the right and down
    constexpr double shift_x = 0.1;
    constexpr double shift_y = 0.1;

    // Translate to relative coordinates
    double rel_x = get_relative_coord (coords.longitude, min_longitude, max_longitude);
    double rel_y = get_relative_coord (coords.latitude, min_latitude, max_latitude);

    // Reverse y coordinate to fit our coordinate system (switch "north" and "south")
    rel_y = 1 - rel_y;

    // Translate from relative coordinate to absolute pixel values in our coordinate system
    return {
        translate_coord (shift_x, rel_x, width, zoom_x),
        translate_coord (shift_y, rel_y, height, zoom_y)
    };
}

double Main::get_latitude (const Node &node) {
    return node.coordinates.latitude;
}

double Main::get_longitude (const Node &node) {
    return node.coordinates.longitude;
}

void Main::apply_get_node_value_func(GetNodeValueFunc func, decltype(nodes)::iterator it, double &value) {
    auto &node = it->second;
    value = func (node);
}

void Main::find_node_minmax_values (GetNodeValueFunc func, double &min_value, double &max_value) {
    auto minmax = minmax_element(begin(nodes), end(nodes), [func] (const auto &l, const auto &r) {
        return func (l.second) < func (r.second);
    });

    apply_get_node_value_func(func, minmax.first, min_value);
    apply_get_node_value_func(func, minmax.second, max_value);
}

void Main::set_minmax_values() {
    find_node_minmax_values (get_latitude, min_latitude, max_latitude);
    find_node_minmax_values (get_longitude, min_longitude, max_longitude);
}

void Main::on_combo_changed () {
    drawing_area.queue_draw ();
}

Glib::RefPtr<Gtk::ListStore> Main::create_combo_box_model (Gtk::ComboBox &combo_box) {
    Glib::RefPtr<Gtk::ListStore> tree_model = Gtk::ListStore::create(columns);
    combo_box.set_model(tree_model);
    combo_box.pack_start(name_column);
    combo_box.signal_changed().connect(sigc::mem_fun(*this, &Main::on_combo_changed));
    fill_store (tree_model);
    return tree_model;
}

void Main::init_label (Gtk::Label &label, const std::string &text) {
    from_label.set_text(text);
    from_label.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_START);
}

void Main::init () {
    init_columns();

    window.set_default_size(200, 200);
    window.set_title("Navi für Lübeck");

    Glib::RefPtr<Gtk::ListStore> from_tree_model = create_combo_box_model (from_combo_box);
    from_combo_box.set_active(0);

    Glib::RefPtr<Gtk::ListStore> to_tree_model = create_combo_box_model (to_combo_box);
    to_combo_box.set_active(2);

    drawing_area.signal_draw().connect(sigc::mem_fun(*this, &Main::on_draw));

    init_label(from_label, "Startpunkt");
    init_label(to_label, "Zielpunkt");
    init_label(map_label, "Karte");

    init_route_columns();
    init_route_tree_view();

    result_paned.pack1 (route_tree_view, false, true);
    result_paned.pack2 (drawing_area, true, true);

    init_combo_box_box();

    window.add (combo_box_box);
    window.show_all();
}

void Main::init_combo_box_box() {
    combo_box_box.set_orientation(Gtk::ORIENTATION_VERTICAL);
    combo_box_box.pack_start (from_label, false, false);
    combo_box_box.pack_start (from_combo_box, false, false);
    combo_box_box.pack_start (to_label, false, false);
    combo_box_box.pack_start (to_combo_box, false, false);
    combo_box_box.pack_start (map_label, false, false);
    combo_box_box.pack_start (result_paned, true, true);
}

void Main::init_route_tree_view() {
    route_tree_model = Gtk::ListStore::create(route_columns);
    route_tree_view.set_model(route_tree_model);
    route_tree_view.append_column("Punkt", route_node_name_column);
    route_tree_view.append_column("Abschnitt", route_node_length_column);
    route_tree_view.append_column("Gesamt", route_node_distance_column);
    route_tree_view.set_size_request(300, -1);
}

void Main::init_route_columns() {
    route_columns.add(route_node_name_column);
    route_columns.add(route_node_distance_column);
    route_columns.add(route_node_length_column);
}

void Main::init_columns() {
    columns.add (name_column);
    columns.add (node_column);
}

Main::Main (std::map<int, Node> nodes) : nodes(nodes) {
    set_minmax_values();
}

Gtk::Window& Main::get_window () {
    return window;
}

int main(int argc, char* argv[] ) {
    std::setlocale(LC_ALL, "C"); // C uses "." as decimal-point separator
    std::cout.precision(17);

    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.examples.base");

    const std::string json_filename = "../daten_hl_altstadt_routenplaner_koordinaten_format.json";
    JsonLoader test;
    auto nodes = test.run(json_filename);

    Main main (nodes);
    main.init ();
    return app->run (main.get_window ());
}