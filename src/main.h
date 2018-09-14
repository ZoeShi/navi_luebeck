//
// Created by l on 14.09.18.
//

#ifndef NAVI_LUEBECK_MAIN_H
#define NAVI_LUEBECK_MAIN_H

#include <map>
#include <list>
#include <optional>
#include <gtkmm/treemodel.h>
#include <gtkmm/combobox.h>
#include <gtkmm/box.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/paned.h>
#include <gtkmm/liststore.h>
#include <gtkmm/window.h>
#include "JsonLoader.h"

struct Point {
    int x;
    int y;
};

enum class ObjectKind {
    Node,
    Edge,
    NodeName
};

struct Color {
  double red;
  double green;
  double blue;
};

class Main {
private:
    std::map<int, Node> nodes;

    double min_latitude;
    double max_latitude;
    double min_longitude;
    double max_longitude;

    int width;
    int height;

    std::list<int> route;

    Gtk::Window window;
    Gtk::TreeModel::ColumnRecord columns;
    Gtk::TreeModelColumn<Glib::ustring> name_column;
    Gtk::TreeModelColumn<Node*> node_column;
    Gtk::ComboBox from_combo_box;
    Gtk::Label from_label;
    Gtk::ComboBox to_combo_box;
    Gtk::Label to_label;
    Gtk::Box combo_box_box;
    Gtk::Label map_label;
    Gtk::DrawingArea drawing_area;
    Gtk::Box result_box;
    Gtk::Paned result_paned;
    Gtk::TreeView route_tree_view;
    Gtk::TreeModel::ColumnRecord route_columns;
    Gtk::TreeModelColumn<Glib::ustring> route_node_name_column;
    Gtk::TreeModelColumn<int> route_node_distance_column;
    Gtk::TreeModelColumn<int> route_node_length_column;
    Glib::RefPtr<Gtk::ListStore> route_tree_model;

    void fill_store(Glib::RefPtr<Gtk::ListStore> store);
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& context);
    void fill_route_tree_model ();
    void calculate_route();
    void get_selected_nodes(Node *&from_node, Node *&to_node) const;
    using DrawObjectFunc = std::function<void(const Cairo::RefPtr<Cairo::Context>&, const Node&,
                                              const std::optional<int>&)>;
    void draw_objects (const Cairo::RefPtr<Cairo::Context> &context, DrawObjectFunc func);
    void draw_node_names (const Cairo::RefPtr<Cairo::Context> &context);
    void draw_nodes(const Cairo::RefPtr<Cairo::Context> &context);
    void draw_edges(const Cairo::RefPtr<Cairo::Context> &context);
    std::optional<int> route_position (int id) const;
    void set_color (const Cairo::RefPtr<Cairo::Context> &context, ObjectKind kind, bool on_route);
    using DrawingFunc = std::function<void(const Cairo::RefPtr<Cairo::Context> &context)>;
    void draw (const Cairo::RefPtr<Cairo::Context> &context, const DrawingFunc &func, ObjectKind kind, bool on_route);
    void move_to (const Cairo::RefPtr<Cairo::Context> &context, const Node& node);
    void line_to (const Cairo::RefPtr<Cairo::Context> &context, const Node& node);
    void draw_edge(const Cairo::RefPtr<Cairo::Context> &context, const Node &source_node,
                   const Node &target_node, bool on_route);
    void draw_node(const Cairo::RefPtr<Cairo::Context> &context, const Node &node,
                   bool on_route);
    void draw_node_name(const Cairo::RefPtr<Cairo::Context> &context, const Node &node,
                        const std::optional<int> &route_pos);
    double get_relative_coord (double value, double min_value, double max_value);
    int translate_coord (double shift, double value, double length, double zoom) const;
    Point get_xy_from_coords (const Coordinates &coords);
    static double get_latitude (const Node &node);
    static double get_longitude (const Node &node);
    using GetNodeValueFunc = double (*)(const Node&);
    void apply_get_node_value_func(GetNodeValueFunc func, decltype(nodes)::iterator it, double &value);
    void find_node_minmax_values (GetNodeValueFunc func, double &min_value, double &max_value);
    void set_minmax_values();
    void on_combo_changed ();
    Glib::RefPtr<Gtk::ListStore> create_combo_box_model (Gtk::ComboBox &combo_box);
    void init_label (Gtk::Label &label, const std::string &text);
    void init_combo_box_box();
    void init_route_tree_view();
    void init_route_columns();
    void init_columns();
public:
    Main (std::map<int, Node> nodes);
    void init ();
    Gtk::Window& get_window ();
};

#endif //NAVI_LUEBECK_MAIN_H
