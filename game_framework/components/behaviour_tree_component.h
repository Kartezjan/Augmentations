#pragma once
#include <vector>
#include <unordered_map>

#include "entity_system/entity.h"
#include "misc/timer.h"

namespace components {
	struct behaviour_tree  {
		struct task;
		struct decorator;

		/* behaviour interface */
		struct behaviour {
			enum status {
				RUNNING,
				FAILURE,
				SUCCESS,
				INTERRUPTED
			};
		};

		/* base behaviour composite */
		struct composite : behaviour {
			typedef std::vector<composite*> behaviors;

			enum type {
				SEQUENCER,
				SELECTOR,
				CONCURRENT
			};

			std::string name;

			int concurrent_return;

			int default_return;
			int node_type;

			int skip_to_running_child;
			behaviors children;
			
			std::string get_type_str() const;
			std::string get_result_str(int result) const;

			int begin_traversal(task&);

			struct update_input {
				task* current_task;
				composite* parent;
				std::vector<composite*>* parent_chain;

				size_t child_index;
				update_input(task* current_task = nullptr) : current_task(current_task), child_index(0u), parent(nullptr) {}
			};

			int update(update_input);

			/* virtuals are for mocking and decorators */
			virtual int tick(update_input);
			virtual int traverse(task&);
			
			/* these functions only call the script delegates */
			virtual void on_enter(task&);
			virtual void on_exit(task&, int exit_code);
			virtual int on_update(task&);

			bool is_currently_running(const task&) const;
			bool is_running_parent(const task&) const;
			static void set_running(update_input, int status = status::FAILURE);

			/* actual implemented behaviours */
			std::function<void(augs::entity_id, task&)> enter_callback;
			std::function<int(augs::entity_id, task&)> update_callback;
			std::function<void(augs::entity_id, int)> exit_callback;

			composite();

			void add_child(composite*);

			decorator* decorator_chain;
		};

		/* an inverse decorator to keep the tree structure uninvaded */
		struct decorator {
			decorator* next_decorator;
			decorator() : next_decorator(nullptr) {}

			virtual int update(composite* current, composite::update_input);
		};

		struct timer_decorator : decorator {
			float maximum_running_time_ms;
			float return_failure_for_the_first_ms;
			
			timer_decorator();
			int update(composite* current, composite::update_input) override;
		};

		struct task {
			void interrupt_other_runner(int status = composite::status::FAILURE);
			void interrupt_runner(int status = composite::status::FAILURE);

			augs::entity_id subject;

			composite* running_parent_node;

			composite* callbacking_node = nullptr;
			size_t running_index;
			std::vector<composite*> parent_chain;
			std::vector<composite*> running_node_parent_chain;

			bool operator==(const task&) const;

			task();
			
			augs::timer since_entered;
		};

		struct tree_instance {
			composite* starting_node;
			task task_instance;
		};

		std::vector<tree_instance> trees;
	
		void add_tree(composite* starting_node) {
			tree_instance t;
			t.starting_node = starting_node;
			trees.push_back(t);
		}
	};
}

