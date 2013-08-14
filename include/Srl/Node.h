#ifndef SRL_NODE_H
#define SRL_NODE_H

#include "Enums.h"
#include "Common.h"
#include "Value.h"
#include "In.h"
#include "Resolve.h"
#include "TpTools.h"
#include "Heap.h"
#include "Environment.h"
#include "Tree.h"

namespace Srl {

    class String;
    struct Parser;

    class Node {

        friend class Tree;
        template<class T, class U>
        friend struct Lib::Switch;
        friend struct Lib::Environment;

    public :
        Node(Tree& tree_) : Node(&tree_, Type::Object) { }

        template<class Head, class... Tail>
        Node& insert (const String& field_name, const Head& head, const Tail&... tail);

        template<class T>
        Node& insert (const String& field_name, const T& value);

        template<class T>
        Node& insert (const String& field_name, const std::initializer_list<T>& elements);

        template<class T>
        Node& insert (const T& value);

        inline Node& insert();

        template<class T> T unwrap ();
        template<class T> void paste (T& o);

        template<class T>
        typename std::enable_if<!TpTools::is_scope(Lib::Switch<T>::type), void>::type
        paste_field (const String& field_name, T& o);

        template<class T>
        typename std::enable_if<!TpTools::is_scope(Lib::Switch<T>::type), void>::type
        paste_field (size_t index, T& o);

        template<class T>
        typename std::enable_if<TpTools::is_scope(Lib::Switch<T>::type), void>::type
        paste_field (const String& field_name, T& o);

        template<class T>
        typename std::enable_if<TpTools::is_scope(Lib::Switch<T>::type), void>::type
        paste_field (size_t index, T& o);

        template<class T> T unwrap_field (const String& field_name);
        template<class T> T unwrap_field (size_t index);

        Node& node (const String& name);
        Node& node (size_t index);

        Value& value (const String& name);
        Value& value (size_t index);

        std::list<Node*>  find_nodes  (const String& name, bool recursive = false);
        std::list<Value*> find_values (const String& name, bool recursive = false);

        std::list<Node*>  all_nodes  (bool recursive = false);
        std::list<Value*> all_values (bool recursive = false);

        bool has_node  (const String& name);
        bool has_value (const String& name);

        void foreach_node  (const std::function<void(Node&)>& fnc, bool recursive = false);
        void foreach_value (const std::function<void(Value&)>& fnc, bool recursive = false);

        void remove_node (Node* node);
        void remove_node (size_t index);
        void remove_node (const String& name);

        void remove_value (Value* value);
        void remove_value (size_t index);
        void remove_value (const String& name);

        inline size_t num_nodes()    const;
        inline size_t num_values()   const;
        inline Type   type()         const;
        inline const  String& name() const;

        template<class TParser>
        std::vector<uint8_t> to_source(TParser&& parser = TParser());

        template<class TParser>
        void to_source(Lib::Out::Source source, TParser&& parser = TParser());

    private:
        Node(Tree* tree, Type type_ = Type::Object, bool parsed_ = true)
            : env(tree->env.get()), nodes(env->heap), values(env->heap),
              scope_type(type_), parsed(parsed_) { }

        Lib::Environment* env;
        Lib::Items<Node>  nodes;
        Lib::Items<Value> values;

        const String* name_ptr = nullptr;

        Type          scope_type;
        bool          parsed;

        template<class... Args>
        void open_scope (void (*Insert)(Node& node, const Args&... args),
                         Type node_type, const String& name, const Args&... args);

        std::pair<bool, size_t> insert_shared (const void* obj);
        std::pair<bool, std::shared_ptr<void>*>  find_shared (size_t key, const std::function<std::shared_ptr<void>(void)>& create);

        Node& insert_node  (const Node& node, const String& name);
        Node& insert_node  (Type type, const String& name);
        void  insert_value (const Value& value, const String& name);

        void  to_source   ();
        void  read_source ();

        void  consume_scope ();
        Node  consume_node  (bool throw_exception);
        Value consume_value (bool throw_exception); 

        Node  consume_node  (const String& name);
        Value consume_value (const String& name);

        template<class T>
        typename std::enable_if<!TpTools::is_scope(Lib::Switch<T>::type), Value>::type
        consume_item();

        template<class T>
        typename std::enable_if<TpTools::is_scope(Lib::Switch<T>::type), Node>::type
        consume_item();

        template<class T>
        typename std::enable_if<!TpTools::is_scope(Lib::Switch<T>::type), Lib::Items<Value>&>::type
        items();

        template<class T>
        typename std::enable_if<TpTools::is_scope(Lib::Switch<T>::type), Lib::Items<Node>&>::type
        items();
    };
}

#endif
