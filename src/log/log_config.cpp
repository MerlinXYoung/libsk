// Generated by tool, DO NOT EDIT!!!

#include "log_config.h"
#include "pugixml.hpp"
#include "utility/types.h"
#include "utility/assert_helper.h"
#include "utility/string_helper.h"

static int load_from_xml_node(sk::log_config::net_device& value, const pugi::xml_node& node, const char *node_name);
static int load_from_xml_node(std::string& value, const pugi::xml_node& node, const char *node_name);
static int load_from_xml_node(std::vector<sk::log_config::category>& value, const pugi::xml_object_range<pugi::xml_named_node_iterator>& nodes, const char *node_name);
static int load_from_xml_node(sk::log_config& value, const pugi::xml_node& node, const char *node_name);
static int load_from_xml_node(sk::log_config::file_device& value, const pugi::xml_node& node, const char *node_name);
static int load_from_xml_node(u32& value, const pugi::xml_node& node, const char *node_name);
static int load_from_xml_node(std::vector<sk::log_config::file_device>& value, const pugi::xml_object_range<pugi::xml_named_node_iterator>& nodes, const char *node_name);
static int load_from_xml_node(sk::log_config::category& value, const pugi::xml_node& node, const char *node_name);
static int load_from_xml_node(std::vector<sk::log_config::net_device>& value, const pugi::xml_object_range<pugi::xml_named_node_iterator>& nodes, const char *node_name);


static int load_from_xml_node(sk::log_config::net_device& value, const pugi::xml_node& node, const char *node_name) {
    int ret = 0;
    if (!node) {
        fprintf(stderr, "node %s not found.\n", node_name);
        return -EINVAL;
    }

    // read fields

    ret = load_from_xml_node(value.address, node.child("address"), "address");
    if (ret != 0)
        return ret;

    ret = load_from_xml_node(value.port, node.child("port"), "port");
    if (ret != 0)
        return ret;

    return 0;
}

static int load_from_xml_node(std::string& value, const pugi::xml_node& node, const char *node_name) {
    value.clear();
    if (!node) {
        fprintf(stderr, "node %s not found.\n", node_name);
        return -EINVAL;
    }
    value = node.text().get();

    return 0;
}

static int load_from_xml_node(std::vector<sk::log_config::category>& value, const pugi::xml_object_range<pugi::xml_named_node_iterator>& nodes, const char *node_name) {
    value.clear();
    for (auto it = nodes.begin(), end = nodes.end(); it != end; ++it) {
        const pugi::xml_node n = *it;
        sk::log_config::category obj;
        int ret = load_from_xml_node(obj, n, node_name);
        if (ret != 0)
            return ret;

        value.push_back(obj);
    }

    /*
     * do NOT do this check, some selective configurations might
     * be totally optional, so it's possible nothing is provided
     */
    // if (value.size() <= 0) {
    //     fprintf(stderr, "node %s not found.\n", node_name);
    //     return -EINVAL;
    // }

    return 0;
}

static int load_from_xml_node(sk::log_config& value, const pugi::xml_node& node, const char *node_name) {
    int ret = 0;
    if (!node) {
        fprintf(stderr, "node %s not found.\n", node_name);
        return -EINVAL;
    }

    // read fields

    ret = load_from_xml_node(value.categories, node.children("categories"), "categories");
    if (ret != 0)
        return ret;

    return 0;
}

static int load_from_xml_node(sk::log_config::file_device& value, const pugi::xml_node& node, const char *node_name) {
    int ret = 0;
    if (!node) {
        fprintf(stderr, "node %s not found.\n", node_name);
        return -EINVAL;
    }

    // read fields

    ret = load_from_xml_node(value.path, node.child("path"), "path");
    if (ret != 0)
        return ret;

    ret = load_from_xml_node(value.max_size, node.child("max_size"), "max_size");
    if (ret != 0)
        return ret;

    ret = load_from_xml_node(value.max_rotation, node.child("max_rotation"), "max_rotation");
    if (ret != 0)
        return ret;

    return 0;
}

static int load_from_xml_node(u32& value, const pugi::xml_node& node, const char *node_name) {
    value = 0;
    if (!node) {
        fprintf(stderr, "node %s not found.\n", node_name);
        return -EINVAL;
    }

    int ret = sk::string_traits<u32>::from_string(node.text().get(), value);
    if (ret != 0) {
        fprintf(stderr, "cannot convert node %s to integer.\n", node_name);
        return ret;
    }

    return 0;
}

static int load_from_xml_node(std::vector<sk::log_config::file_device>& value, const pugi::xml_object_range<pugi::xml_named_node_iterator>& nodes, const char *node_name) {
    value.clear();
    for (auto it = nodes.begin(), end = nodes.end(); it != end; ++it) {
        const pugi::xml_node n = *it;
        sk::log_config::file_device obj;
        int ret = load_from_xml_node(obj, n, node_name);
        if (ret != 0)
            return ret;

        value.push_back(obj);
    }

    /*
     * do NOT do this check, some selective configurations might
     * be totally optional, so it's possible nothing is provided
     */
    // if (value.size() <= 0) {
    //     fprintf(stderr, "node %s not found.\n", node_name);
    //     return -EINVAL;
    // }

    return 0;
}

static int load_from_xml_node(sk::log_config::category& value, const pugi::xml_node& node, const char *node_name) {
    int ret = 0;
    if (!node) {
        fprintf(stderr, "node %s not found.\n", node_name);
        return -EINVAL;
    }

    // read fields

    ret = load_from_xml_node(value.name, node.child("name"), "name");
    if (ret != 0)
        return ret;

    ret = load_from_xml_node(value.pattern, node.child("pattern"), "pattern");
    if (ret != 0)
        return ret;

    ret = load_from_xml_node(value.level, node.child("level"), "level");
    if (ret != 0)
        return ret;

    ret = load_from_xml_node(value.fdev_list, node.children("fdev_list"), "fdev_list");
    if (ret != 0)
        return ret;

    ret = load_from_xml_node(value.ndev_list, node.children("ndev_list"), "ndev_list");
    if (ret != 0)
        return ret;

    return 0;
}

static int load_from_xml_node(std::vector<sk::log_config::net_device>& value, const pugi::xml_object_range<pugi::xml_named_node_iterator>& nodes, const char *node_name) {
    value.clear();
    for (auto it = nodes.begin(), end = nodes.end(); it != end; ++it) {
        const pugi::xml_node n = *it;
        sk::log_config::net_device obj;
        int ret = load_from_xml_node(obj, n, node_name);
        if (ret != 0)
            return ret;

        value.push_back(obj);
    }

    /*
     * do NOT do this check, some selective configurations might
     * be totally optional, so it's possible nothing is provided
     */
    // if (value.size() <= 0) {
    //     fprintf(stderr, "node %s not found.\n", node_name);
    //     return -EINVAL;
    // }

    return 0;
}

int sk::log_config::load_from_xml_file(const char *filename) {
    assert_retval(filename, -1);

    pugi::xml_document doc;
    auto ok = doc.load_file(filename);
    if (!ok) {
        fprintf(stderr, "load file %s error: %s.\n", filename, ok.description());
        return -EINVAL;
    }

    pugi::xml_node root = doc.child("log_config");
    if (!root) {
        fprintf(stderr, "root node <log_config> not found.\n");
        return -EINVAL;
    }

    return load_from_xml_node(*this, root, "log_config");
}