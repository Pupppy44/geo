#include "tree.h"

namespace geo {
    namespace tree {
        tree::tree() {};

        tree::~tree() {};

        void tree::add_object(std::shared_ptr<object> obj) {
            objects.push_back(obj);
        }

        void tree::remove_object(std::shared_ptr<object> obj) {
            // Uses a for loop to iterate through the objects vector and remove the object
            for (int i = 0; i < objects.size(); i++) {
                if (objects[i]->id() == obj->id()) {
                    objects.erase(objects.begin() + i);
                }
            }
        }

        std::shared_ptr<object> tree::get_object_by_id(std::string id) {
            for (std::shared_ptr<object> obj : objects) {
                if (obj->id() == id) {
                    return obj;
                }
            }

			return nullptr;
        }

        std::vector<std::shared_ptr<object>> tree::get_objects() {
            return objects;
        }

		void tree::clear_objects() {
            objects.clear();
		}
    }
}