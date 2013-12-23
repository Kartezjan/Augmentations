#pragma once
#include <vector>
#include <unordered_map>
#include "entity_system/component.h"
#include "entity_system/entity_ptr.h"
#include "../../../utility/timer.h"

namespace components {
	struct behaviour_tree : public augmentations::entity_system::component {
		struct task;
		struct decorator;

		/* behaviour interface */
		struct behaviour {
			enum status {
				RUNNING,
				FAILURE,
				SUCCESS
			};
		};

		/* base behaviour composite */
		struct composite : behaviour {
			typedef std::vector<composite*> behaviors;

			enum type {
				SEQUENCER,
				SELECTOR
			};

			std::string name;

			int default_return;
			int node_type;
			behaviors children;

			int begin_traversal(task&);

			struct update_input {
				task* current_task;
				composite* parent;
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
			static void interrupt_running(update_input, int status = status::FAILURE);

			/* actual implemented behaviours */
			luabind::object enter_callback;
			luabind::object exit_callback;
			luabind::object update_callback;

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
			
			timer_decorator();
			int update(composite* current, composite::update_input) override;
		};

		struct task {
			augmentations::entity_system::entity* subject;

			composite* running_parent_node;
			size_t running_index;

			bool operator==(const task&) const;

			task();
			
			augmentations::util::timer since_entered;
		};

		composite* starting_node;
		task task_instance;
	};
}
