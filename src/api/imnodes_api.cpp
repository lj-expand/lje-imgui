#include "imnodes_api.hpp"
#include "../globals.hpp"
#include <imnodes.h>
#include <vector>

namespace imnodes_api {

void init() {
  ImNodes::CreateContext();
}

void shutdown() {
  ImNodes::DestroyContext();
}

// Editor
static int begin_node_editor(lua_State *L) {
  (void)L;
  ImNodes::BeginNodeEditor();
  return 0;
}

static int end_node_editor(lua_State *L) {
  (void)L;
  ImNodes::EndNodeEditor();
  return 0;
}

static int minimap(lua_State *L) {
  auto lua = g_api->lua;
  float size_fraction = 0.2f;
  int location = ImNodesMiniMapLocation_TopRight;

  int nargs = lua->gettop(L);
  if (nargs >= 1)
    size_fraction = static_cast<float>(lua->tonumber(L, 1));
  if (nargs >= 2)
    location = static_cast<int>(lua->tonumber(L, 2));
  lua->pop(L, nargs);

  ImNodes::MiniMap(size_fraction, static_cast<ImNodesMiniMapLocation>(location));
  return 0;
}

// Nodes
static int begin_node(lua_State *L) {
  auto lua = g_api->lua;
  int id = static_cast<int>(lua->tonumber(L, 1));
  lua->pop(L, 1);
  ImNodes::BeginNode(id);
  return 0;
}

static int end_node(lua_State *L) {
  (void)L;
  ImNodes::EndNode();
  return 0;
}

static int begin_node_titlebar(lua_State *L) {
  (void)L;
  ImNodes::BeginNodeTitleBar();
  return 0;
}

static int end_node_titlebar(lua_State *L) {
  (void)L;
  ImNodes::EndNodeTitleBar();
  return 0;
}

// Attributes (pins)
static int begin_input_attribute(lua_State *L) {
  auto lua = g_api->lua;
  int id = static_cast<int>(lua->tonumber(L, 1));
  int shape = ImNodesPinShape_CircleFilled;

  int nargs = lua->gettop(L);
  if (nargs >= 2)
    shape = static_cast<int>(lua->tonumber(L, 2));
  lua->pop(L, nargs);

  ImNodes::BeginInputAttribute(id, static_cast<ImNodesPinShape>(shape));
  return 0;
}

static int end_input_attribute(lua_State *L) {
  (void)L;
  ImNodes::EndInputAttribute();
  return 0;
}

static int begin_output_attribute(lua_State *L) {
  auto lua = g_api->lua;
  int id = static_cast<int>(lua->tonumber(L, 1));
  int shape = ImNodesPinShape_CircleFilled;

  int nargs = lua->gettop(L);
  if (nargs >= 2)
    shape = static_cast<int>(lua->tonumber(L, 2));
  lua->pop(L, nargs);

  ImNodes::BeginOutputAttribute(id, static_cast<ImNodesPinShape>(shape));
  return 0;
}

static int end_output_attribute(lua_State *L) {
  (void)L;
  ImNodes::EndOutputAttribute();
  return 0;
}

static int begin_static_attribute(lua_State *L) {
  auto lua = g_api->lua;
  int id = static_cast<int>(lua->tonumber(L, 1));
  lua->pop(L, 1);
  ImNodes::BeginStaticAttribute(id);
  return 0;
}

static int end_static_attribute(lua_State *L) {
  (void)L;
  ImNodes::EndStaticAttribute();
  return 0;
}

// Links
static int link(lua_State *L) {
  auto lua = g_api->lua;
  int link_id = static_cast<int>(lua->tonumber(L, 1));
  int start_attr = static_cast<int>(lua->tonumber(L, 2));
  int end_attr = static_cast<int>(lua->tonumber(L, 3));
  lua->pop(L, 3);
  ImNodes::Link(link_id, start_attr, end_attr);
  return 0;
}

// Interaction queries
static int is_link_created(lua_State *L) {
  auto lua = g_api->lua;
  int start_attr, end_attr;
  bool created = ImNodes::IsLinkCreated(&start_attr, &end_attr);
  lua->pushboolean(L, created);
  lua->pushnumber(L, start_attr);
  lua->pushnumber(L, end_attr);
  return 3;
}

static int is_link_destroyed(lua_State *L) {
  auto lua = g_api->lua;
  int link_id;
  bool destroyed = ImNodes::IsLinkDestroyed(&link_id);
  lua->pushboolean(L, destroyed);
  lua->pushnumber(L, link_id);
  return 2;
}

static int is_link_started(lua_State *L) {
  auto lua = g_api->lua;
  int started_at_attr;
  bool started = ImNodes::IsLinkStarted(&started_at_attr);
  lua->pushboolean(L, started);
  lua->pushnumber(L, started_at_attr);
  return 2;
}

static int is_link_dropped(lua_State *L) {
  auto lua = g_api->lua;
  int started_at_attr;
  bool including_detached = true;

  int nargs = lua->gettop(L);
  if (nargs >= 1)
    including_detached = lua->toboolean(L, 1);
  lua->pop(L, nargs);

  bool dropped = ImNodes::IsLinkDropped(&started_at_attr, including_detached);
  lua->pushboolean(L, dropped);
  lua->pushnumber(L, started_at_attr);
  return 2;
}

static int is_link_hovered(lua_State *L) {
  auto lua = g_api->lua;
  int link_id;
  bool hovered = ImNodes::IsLinkHovered(&link_id);
  lua->pushboolean(L, hovered);
  lua->pushnumber(L, link_id);
  return 2;
}

static int is_node_hovered(lua_State *L) {
  auto lua = g_api->lua;
  int node_id;
  bool hovered = ImNodes::IsNodeHovered(&node_id);
  lua->pushboolean(L, hovered);
  lua->pushnumber(L, node_id);
  return 2;
}

static int is_pin_hovered(lua_State *L) {
  auto lua = g_api->lua;
  int attr_id;
  bool hovered = ImNodes::IsPinHovered(&attr_id);
  lua->pushboolean(L, hovered);
  lua->pushnumber(L, attr_id);
  return 2;
}

// Selection
static int num_selected_nodes(lua_State *L) {
  auto lua = g_api->lua;
  lua->pushnumber(L, ImNodes::NumSelectedNodes());
  return 1;
}

static int num_selected_links(lua_State *L) {
  auto lua = g_api->lua;
  lua->pushnumber(L, ImNodes::NumSelectedLinks());
  return 1;
}

static int get_selected_nodes(lua_State *L) {
  auto lua = g_api->lua;
  int count = ImNodes::NumSelectedNodes();
  if (count == 0) {
    lua->createtable(L, 0, 0);
    return 1;
  }

  std::vector<int> nodes(count);
  ImNodes::GetSelectedNodes(nodes.data());

  lua->createtable(L, count, 0);
  for (int i = 0; i < count; i++) {
    lua->pushnumber(L, nodes[i]);
    lua->rawseti(L, -2, i + 1);
  }
  return 1;
}

static int get_selected_links(lua_State *L) {
  auto lua = g_api->lua;
  int count = ImNodes::NumSelectedLinks();
  if (count == 0) {
    lua->createtable(L, 0, 0);
    return 1;
  }

  std::vector<int> links(count);
  ImNodes::GetSelectedLinks(links.data());

  lua->createtable(L, count, 0);
  for (int i = 0; i < count; i++) {
    lua->pushnumber(L, links[i]);
    lua->rawseti(L, -2, i + 1);
  }
  return 1;
}

static int clear_node_selection(lua_State *L) {
  auto lua = g_api->lua;
  int nargs = lua->gettop(L);
  if (nargs >= 1) {
    int node_id = static_cast<int>(lua->tonumber(L, 1));
    lua->pop(L, nargs);
    ImNodes::ClearNodeSelection(node_id);
  } else {
    ImNodes::ClearNodeSelection();
  }
  return 0;
}

static int clear_link_selection(lua_State *L) {
  auto lua = g_api->lua;
  int nargs = lua->gettop(L);
  if (nargs >= 1) {
    int link_id = static_cast<int>(lua->tonumber(L, 1));
    lua->pop(L, nargs);
    ImNodes::ClearLinkSelection(link_id);
  } else {
    ImNodes::ClearLinkSelection();
  }
  return 0;
}

static int select_node(lua_State *L) {
  auto lua = g_api->lua;
  int node_id = static_cast<int>(lua->tonumber(L, 1));
  lua->pop(L, 1);
  ImNodes::SelectNode(node_id);
  return 0;
}

static int select_link(lua_State *L) {
  auto lua = g_api->lua;
  int link_id = static_cast<int>(lua->tonumber(L, 1));
  lua->pop(L, 1);
  ImNodes::SelectLink(link_id);
  return 0;
}

static int is_node_selected(lua_State *L) {
  auto lua = g_api->lua;
  int node_id = static_cast<int>(lua->tonumber(L, 1));
  lua->pop(L, 1);
  lua->pushboolean(L, ImNodes::IsNodeSelected(node_id));
  return 1;
}

static int is_link_selected(lua_State *L) {
  auto lua = g_api->lua;
  int link_id = static_cast<int>(lua->tonumber(L, 1));
  lua->pop(L, 1);
  lua->pushboolean(L, ImNodes::IsLinkSelected(link_id));
  return 1;
}

// Node positioning
static int set_node_position(lua_State *L) {
  auto lua = g_api->lua;
  int node_id = static_cast<int>(lua->tonumber(L, 1));
  float x = static_cast<float>(lua->tonumber(L, 2));
  float y = static_cast<float>(lua->tonumber(L, 3));
  lua->pop(L, 3);
  ImNodes::SetNodeScreenSpacePos(node_id, ImVec2(x, y));
  return 0;
}

static int set_node_editor_pos(lua_State *L) {
  auto lua = g_api->lua;
  int node_id = static_cast<int>(lua->tonumber(L, 1));
  float x = static_cast<float>(lua->tonumber(L, 2));
  float y = static_cast<float>(lua->tonumber(L, 3));
  lua->pop(L, 3);
  ImNodes::SetNodeEditorSpacePos(node_id, ImVec2(x, y));
  return 0;
}

static int set_node_grid_pos(lua_State *L) {
  auto lua = g_api->lua;
  int node_id = static_cast<int>(lua->tonumber(L, 1));
  float x = static_cast<float>(lua->tonumber(L, 2));
  float y = static_cast<float>(lua->tonumber(L, 3));
  lua->pop(L, 3);
  ImNodes::SetNodeGridSpacePos(node_id, ImVec2(x, y));
  return 0;
}

static int get_node_position(lua_State *L) {
  auto lua = g_api->lua;
  int node_id = static_cast<int>(lua->tonumber(L, 1));
  lua->pop(L, 1);
  ImVec2 pos = ImNodes::GetNodeScreenSpacePos(node_id);
  lua->pushnumber(L, pos.x);
  lua->pushnumber(L, pos.y);
  return 2;
}

static int get_node_editor_pos(lua_State *L) {
  auto lua = g_api->lua;
  int node_id = static_cast<int>(lua->tonumber(L, 1));
  lua->pop(L, 1);
  ImVec2 pos = ImNodes::GetNodeEditorSpacePos(node_id);
  lua->pushnumber(L, pos.x);
  lua->pushnumber(L, pos.y);
  return 2;
}

static int get_node_grid_pos(lua_State *L) {
  auto lua = g_api->lua;
  int node_id = static_cast<int>(lua->tonumber(L, 1));
  lua->pop(L, 1);
  ImVec2 pos = ImNodes::GetNodeGridSpacePos(node_id);
  lua->pushnumber(L, pos.x);
  lua->pushnumber(L, pos.y);
  return 2;
}

static int get_node_dimensions(lua_State *L) {
  auto lua = g_api->lua;
  int node_id = static_cast<int>(lua->tonumber(L, 1));
  lua->pop(L, 1);
  ImVec2 dims = ImNodes::GetNodeDimensions(node_id);
  lua->pushnumber(L, dims.x);
  lua->pushnumber(L, dims.y);
  return 2;
}

// Editor panning
static int editor_reset_panning(lua_State *L) {
  auto lua = g_api->lua;
  float x = 0, y = 0;
  int nargs = lua->gettop(L);
  if (nargs >= 2) {
    x = static_cast<float>(lua->tonumber(L, 1));
    y = static_cast<float>(lua->tonumber(L, 2));
  }
  lua->pop(L, nargs);
  ImNodes::EditorContextResetPanning(ImVec2(x, y));
  return 0;
}

static int editor_get_panning(lua_State *L) {
  auto lua = g_api->lua;
  ImVec2 pan = ImNodes::EditorContextGetPanning();
  lua->pushnumber(L, pan.x);
  lua->pushnumber(L, pan.y);
  return 2;
}

// Styling
static int push_color_style(lua_State *L) {
  auto lua = g_api->lua;
  int item = static_cast<int>(lua->tonumber(L, 1));
  unsigned int color = static_cast<unsigned int>(lua->tonumber(L, 2));
  lua->pop(L, 2);
  ImNodes::PushColorStyle(static_cast<ImNodesCol>(item), color);
  return 0;
}

static int pop_color_style(lua_State *L) {
  (void)L;
  ImNodes::PopColorStyle();
  return 0;
}

static int push_style_var(lua_State *L) {
  auto lua = g_api->lua;
  int item = static_cast<int>(lua->tonumber(L, 1));
  float value = static_cast<float>(lua->tonumber(L, 2));
  lua->pop(L, 2);
  ImNodes::PushStyleVar(static_cast<ImNodesStyleVar>(item), value);
  return 0;
}

static int push_style_var_vec2(lua_State *L) {
  auto lua = g_api->lua;
  int item = static_cast<int>(lua->tonumber(L, 1));
  float x = static_cast<float>(lua->tonumber(L, 2));
  float y = static_cast<float>(lua->tonumber(L, 3));
  lua->pop(L, 3);
  ImNodes::PushStyleVar(static_cast<ImNodesStyleVar>(item), ImVec2(x, y));
  return 0;
}

static int pop_style_var(lua_State *L) {
  auto lua = g_api->lua;
  int count = 1;
  int nargs = lua->gettop(L);
  if (nargs >= 1)
    count = static_cast<int>(lua->tonumber(L, 1));
  lua->pop(L, nargs);
  ImNodes::PopStyleVar(count);
  return 0;
}

// Attribute flags
static int push_attribute_flag(lua_State *L) {
  auto lua = g_api->lua;
  int flag = static_cast<int>(lua->tonumber(L, 1));
  lua->pop(L, 1);
  ImNodes::PushAttributeFlag(static_cast<ImNodesAttributeFlags>(flag));
  return 0;
}

static int pop_attribute_flag(lua_State *L) {
  (void)L;
  ImNodes::PopAttributeFlag();
  return 0;
}

static int is_editor_hovered(lua_State *L) {
  auto lua = g_api->lua;
  lua->pushboolean(L, ImNodes::IsEditorHovered());
  return 1;
}

void register_all(lua_State *L) {
  auto lua = g_api->lua;

  lua->pushljeenv(L);

  // Create imnodes table
  lua->createtable(L, 0, 0);

  // Editor
  lua->pushcclosure(L, begin_node_editor, 0);
  lua->setfield(L, -2, "begin_node_editor");
  lua->pushcclosure(L, end_node_editor, 0);
  lua->setfield(L, -2, "end_node_editor");
  lua->pushcclosure(L, minimap, 0);
  lua->setfield(L, -2, "minimap");
  lua->pushcclosure(L, is_editor_hovered, 0);
  lua->setfield(L, -2, "is_editor_hovered");

  // Nodes
  lua->pushcclosure(L, begin_node, 0);
  lua->setfield(L, -2, "begin_node");
  lua->pushcclosure(L, end_node, 0);
  lua->setfield(L, -2, "end_node");
  lua->pushcclosure(L, begin_node_titlebar, 0);
  lua->setfield(L, -2, "begin_node_titlebar");
  lua->pushcclosure(L, end_node_titlebar, 0);
  lua->setfield(L, -2, "end_node_titlebar");

  // Attributes
  lua->pushcclosure(L, begin_input_attribute, 0);
  lua->setfield(L, -2, "begin_input_attribute");
  lua->pushcclosure(L, end_input_attribute, 0);
  lua->setfield(L, -2, "end_input_attribute");
  lua->pushcclosure(L, begin_output_attribute, 0);
  lua->setfield(L, -2, "begin_output_attribute");
  lua->pushcclosure(L, end_output_attribute, 0);
  lua->setfield(L, -2, "end_output_attribute");
  lua->pushcclosure(L, begin_static_attribute, 0);
  lua->setfield(L, -2, "begin_static_attribute");
  lua->pushcclosure(L, end_static_attribute, 0);
  lua->setfield(L, -2, "end_static_attribute");

  // Links
  lua->pushcclosure(L, link, 0);
  lua->setfield(L, -2, "link");
  lua->pushcclosure(L, is_link_created, 0);
  lua->setfield(L, -2, "is_link_created");
  lua->pushcclosure(L, is_link_destroyed, 0);
  lua->setfield(L, -2, "is_link_destroyed");
  lua->pushcclosure(L, is_link_started, 0);
  lua->setfield(L, -2, "is_link_started");
  lua->pushcclosure(L, is_link_dropped, 0);
  lua->setfield(L, -2, "is_link_dropped");
  lua->pushcclosure(L, is_link_hovered, 0);
  lua->setfield(L, -2, "is_link_hovered");

  // Hover queries
  lua->pushcclosure(L, is_node_hovered, 0);
  lua->setfield(L, -2, "is_node_hovered");
  lua->pushcclosure(L, is_pin_hovered, 0);
  lua->setfield(L, -2, "is_pin_hovered");

  // Selection
  lua->pushcclosure(L, num_selected_nodes, 0);
  lua->setfield(L, -2, "num_selected_nodes");
  lua->pushcclosure(L, num_selected_links, 0);
  lua->setfield(L, -2, "num_selected_links");
  lua->pushcclosure(L, get_selected_nodes, 0);
  lua->setfield(L, -2, "get_selected_nodes");
  lua->pushcclosure(L, get_selected_links, 0);
  lua->setfield(L, -2, "get_selected_links");
  lua->pushcclosure(L, clear_node_selection, 0);
  lua->setfield(L, -2, "clear_node_selection");
  lua->pushcclosure(L, clear_link_selection, 0);
  lua->setfield(L, -2, "clear_link_selection");
  lua->pushcclosure(L, select_node, 0);
  lua->setfield(L, -2, "select_node");
  lua->pushcclosure(L, select_link, 0);
  lua->setfield(L, -2, "select_link");
  lua->pushcclosure(L, is_node_selected, 0);
  lua->setfield(L, -2, "is_node_selected");
  lua->pushcclosure(L, is_link_selected, 0);
  lua->setfield(L, -2, "is_link_selected");

  // Positioning
  lua->pushcclosure(L, set_node_position, 0);
  lua->setfield(L, -2, "set_node_position");
  lua->pushcclosure(L, set_node_editor_pos, 0);
  lua->setfield(L, -2, "set_node_editor_pos");
  lua->pushcclosure(L, set_node_grid_pos, 0);
  lua->setfield(L, -2, "set_node_grid_pos");
  lua->pushcclosure(L, get_node_position, 0);
  lua->setfield(L, -2, "get_node_position");
  lua->pushcclosure(L, get_node_editor_pos, 0);
  lua->setfield(L, -2, "get_node_editor_pos");
  lua->pushcclosure(L, get_node_grid_pos, 0);
  lua->setfield(L, -2, "get_node_grid_pos");
  lua->pushcclosure(L, get_node_dimensions, 0);
  lua->setfield(L, -2, "get_node_dimensions");

  // Panning
  lua->pushcclosure(L, editor_reset_panning, 0);
  lua->setfield(L, -2, "editor_reset_panning");
  lua->pushcclosure(L, editor_get_panning, 0);
  lua->setfield(L, -2, "editor_get_panning");

  // Styling
  lua->pushcclosure(L, push_color_style, 0);
  lua->setfield(L, -2, "push_color_style");
  lua->pushcclosure(L, pop_color_style, 0);
  lua->setfield(L, -2, "pop_color_style");
  lua->pushcclosure(L, push_style_var, 0);
  lua->setfield(L, -2, "push_style_var");
  lua->pushcclosure(L, push_style_var_vec2, 0);
  lua->setfield(L, -2, "push_style_var_vec2");
  lua->pushcclosure(L, pop_style_var, 0);
  lua->setfield(L, -2, "pop_style_var");
  lua->pushcclosure(L, push_attribute_flag, 0);
  lua->setfield(L, -2, "push_attribute_flag");
  lua->pushcclosure(L, pop_attribute_flag, 0);
  lua->setfield(L, -2, "pop_attribute_flag");

  // Pin shapes
  lua->pushnumber(L, ImNodesPinShape_Circle);
  lua->setfield(L, -2, "PinShape_Circle");
  lua->pushnumber(L, ImNodesPinShape_CircleFilled);
  lua->setfield(L, -2, "PinShape_CircleFilled");
  lua->pushnumber(L, ImNodesPinShape_Triangle);
  lua->setfield(L, -2, "PinShape_Triangle");
  lua->pushnumber(L, ImNodesPinShape_TriangleFilled);
  lua->setfield(L, -2, "PinShape_TriangleFilled");
  lua->pushnumber(L, ImNodesPinShape_Quad);
  lua->setfield(L, -2, "PinShape_Quad");
  lua->pushnumber(L, ImNodesPinShape_QuadFilled);
  lua->setfield(L, -2, "PinShape_QuadFilled");

  // Minimap locations
  lua->pushnumber(L, ImNodesMiniMapLocation_BottomLeft);
  lua->setfield(L, -2, "MiniMapLocation_BottomLeft");
  lua->pushnumber(L, ImNodesMiniMapLocation_BottomRight);
  lua->setfield(L, -2, "MiniMapLocation_BottomRight");
  lua->pushnumber(L, ImNodesMiniMapLocation_TopLeft);
  lua->setfield(L, -2, "MiniMapLocation_TopLeft");
  lua->pushnumber(L, ImNodesMiniMapLocation_TopRight);
  lua->setfield(L, -2, "MiniMapLocation_TopRight");

  // Attribute flags
  lua->pushnumber(L, ImNodesAttributeFlags_None);
  lua->setfield(L, -2, "AttributeFlags_None");
  lua->pushnumber(L, ImNodesAttributeFlags_EnableLinkDetachWithDragClick);
  lua->setfield(L, -2, "AttributeFlags_EnableLinkDetachWithDragClick");
  lua->pushnumber(L, ImNodesAttributeFlags_EnableLinkCreationOnSnap);
  lua->setfield(L, -2, "AttributeFlags_EnableLinkCreationOnSnap");

  // Color style constants
  lua->pushnumber(L, ImNodesCol_NodeBackground);
  lua->setfield(L, -2, "Col_NodeBackground");
  lua->pushnumber(L, ImNodesCol_NodeBackgroundHovered);
  lua->setfield(L, -2, "Col_NodeBackgroundHovered");
  lua->pushnumber(L, ImNodesCol_NodeBackgroundSelected);
  lua->setfield(L, -2, "Col_NodeBackgroundSelected");
  lua->pushnumber(L, ImNodesCol_NodeOutline);
  lua->setfield(L, -2, "Col_NodeOutline");
  lua->pushnumber(L, ImNodesCol_TitleBar);
  lua->setfield(L, -2, "Col_TitleBar");
  lua->pushnumber(L, ImNodesCol_TitleBarHovered);
  lua->setfield(L, -2, "Col_TitleBarHovered");
  lua->pushnumber(L, ImNodesCol_TitleBarSelected);
  lua->setfield(L, -2, "Col_TitleBarSelected");
  lua->pushnumber(L, ImNodesCol_Link);
  lua->setfield(L, -2, "Col_Link");
  lua->pushnumber(L, ImNodesCol_LinkHovered);
  lua->setfield(L, -2, "Col_LinkHovered");
  lua->pushnumber(L, ImNodesCol_LinkSelected);
  lua->setfield(L, -2, "Col_LinkSelected");
  lua->pushnumber(L, ImNodesCol_Pin);
  lua->setfield(L, -2, "Col_Pin");
  lua->pushnumber(L, ImNodesCol_PinHovered);
  lua->setfield(L, -2, "Col_PinHovered");
  lua->pushnumber(L, ImNodesCol_BoxSelector);
  lua->setfield(L, -2, "Col_BoxSelector");
  lua->pushnumber(L, ImNodesCol_BoxSelectorOutline);
  lua->setfield(L, -2, "Col_BoxSelectorOutline");
  lua->pushnumber(L, ImNodesCol_GridBackground);
  lua->setfield(L, -2, "Col_GridBackground");
  lua->pushnumber(L, ImNodesCol_GridLine);
  lua->setfield(L, -2, "Col_GridLine");
  lua->pushnumber(L, ImNodesCol_GridLinePrimary);
  lua->setfield(L, -2, "Col_GridLinePrimary");
  lua->pushnumber(L, ImNodesCol_MiniMapBackground);
  lua->setfield(L, -2, "Col_MiniMapBackground");
  lua->pushnumber(L, ImNodesCol_MiniMapBackgroundHovered);
  lua->setfield(L, -2, "Col_MiniMapBackgroundHovered");
  lua->pushnumber(L, ImNodesCol_MiniMapOutline);
  lua->setfield(L, -2, "Col_MiniMapOutline");
  lua->pushnumber(L, ImNodesCol_MiniMapOutlineHovered);
  lua->setfield(L, -2, "Col_MiniMapOutlineHovered");
  lua->pushnumber(L, ImNodesCol_MiniMapNodeBackground);
  lua->setfield(L, -2, "Col_MiniMapNodeBackground");
  lua->pushnumber(L, ImNodesCol_MiniMapNodeBackgroundHovered);
  lua->setfield(L, -2, "Col_MiniMapNodeBackgroundHovered");
  lua->pushnumber(L, ImNodesCol_MiniMapNodeBackgroundSelected);
  lua->setfield(L, -2, "Col_MiniMapNodeBackgroundSelected");
  lua->pushnumber(L, ImNodesCol_MiniMapNodeOutline);
  lua->setfield(L, -2, "Col_MiniMapNodeOutline");
  lua->pushnumber(L, ImNodesCol_MiniMapLink);
  lua->setfield(L, -2, "Col_MiniMapLink");
  lua->pushnumber(L, ImNodesCol_MiniMapLinkSelected);
  lua->setfield(L, -2, "Col_MiniMapLinkSelected");
  lua->pushnumber(L, ImNodesCol_MiniMapCanvas);
  lua->setfield(L, -2, "Col_MiniMapCanvas");
  lua->pushnumber(L, ImNodesCol_MiniMapCanvasOutline);
  lua->setfield(L, -2, "Col_MiniMapCanvasOutline");

  // Style var constants
  lua->pushnumber(L, ImNodesStyleVar_GridSpacing);
  lua->setfield(L, -2, "StyleVar_GridSpacing");
  lua->pushnumber(L, ImNodesStyleVar_NodeCornerRounding);
  lua->setfield(L, -2, "StyleVar_NodeCornerRounding");
  lua->pushnumber(L, ImNodesStyleVar_NodePadding);
  lua->setfield(L, -2, "StyleVar_NodePadding");
  lua->pushnumber(L, ImNodesStyleVar_NodeBorderThickness);
  lua->setfield(L, -2, "StyleVar_NodeBorderThickness");
  lua->pushnumber(L, ImNodesStyleVar_LinkThickness);
  lua->setfield(L, -2, "StyleVar_LinkThickness");
  lua->pushnumber(L, ImNodesStyleVar_LinkLineSegmentsPerLength);
  lua->setfield(L, -2, "StyleVar_LinkLineSegmentsPerLength");
  lua->pushnumber(L, ImNodesStyleVar_LinkHoverDistance);
  lua->setfield(L, -2, "StyleVar_LinkHoverDistance");
  lua->pushnumber(L, ImNodesStyleVar_PinCircleRadius);
  lua->setfield(L, -2, "StyleVar_PinCircleRadius");
  lua->pushnumber(L, ImNodesStyleVar_PinQuadSideLength);
  lua->setfield(L, -2, "StyleVar_PinQuadSideLength");
  lua->pushnumber(L, ImNodesStyleVar_PinTriangleSideLength);
  lua->setfield(L, -2, "StyleVar_PinTriangleSideLength");
  lua->pushnumber(L, ImNodesStyleVar_PinLineThickness);
  lua->setfield(L, -2, "StyleVar_PinLineThickness");
  lua->pushnumber(L, ImNodesStyleVar_PinHoverRadius);
  lua->setfield(L, -2, "StyleVar_PinHoverRadius");
  lua->pushnumber(L, ImNodesStyleVar_PinOffset);
  lua->setfield(L, -2, "StyleVar_PinOffset");
  lua->pushnumber(L, ImNodesStyleVar_MiniMapPadding);
  lua->setfield(L, -2, "StyleVar_MiniMapPadding");
  lua->pushnumber(L, ImNodesStyleVar_MiniMapOffset);
  lua->setfield(L, -2, "StyleVar_MiniMapOffset");

  // Set imnodes table in ljeenv
  lua->setfield(L, -2, "imnodes");

  lua->pop(L, 1); // Pop ljeenv
}

} // namespace imnodes_api
