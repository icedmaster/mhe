#ifndef _INODE_HPP_
#define _INODE_HPP_

#include "math/matrix.hpp"
#include "video_driver.hpp"
#include "types.hpp"
#include "transform.hpp"
#include "utils/sysutils.hpp"
#include <boost/shared_ptr.hpp>

namespace mhe
{
	class Node;

	class NodeCallback
	{
	public:
		virtual void process(int state, Node* node) = 0;
	};

    // Base renderable class
    class Node : public Transform
    {
        private:
			int priority_;						
			std::string name_;
			std::vector< boost::shared_ptr<Node> > childs_;			

            virtual void draw_impl(boost::shared_ptr<Driver>) = 0;
            virtual void update_impl(cmn::uint) {}

            virtual bool is_alive() const {return true;}

			virtual void set_priority(int pri) {priority_ = pri;}
			virtual int get_priority() const   {return priority_;}	// normal priority	

			virtual void start_impl(cmn::uint) {}	
		virtual Node* clone_impl() const
		{
			return nullptr;
		}				
        public:
			Node() : priority_(2) {}
            virtual ~Node() {}

            void draw(boost::shared_ptr<Driver> driver/*,
                      const boost::shared_ptr<iCamera>& camera = boost::shared_ptr<iCamera>()*/)
            {
				for (size_t i = 0; i < childs_.size(); ++i)
					childs_[i]->draw(driver);
                draw_impl(driver/*, camera*/);
            }

            void update(cmn::uint tick)
            {
                update_impl(tick);
            }

            bool alive() const
            {
                return is_alive();
            }

			void setPriority(int pri)
			{
				set_priority(pri);
			}

			int priority() const
			{
				return get_priority();
			}				

			void set_name(const std::string& name)
			{
				name_ = name;
			}

			const std::string& name() const
			{
				return name_;
			}

			void add_node(boost::shared_ptr<Node> node)
			{
				childs_.push_back(node);
			}

			void start()
			{
				start_impl(utils::get_current_tick());
			}	

			Node* clone() const
			{
				return clone_impl();
			}
    };
	
	// helper classes
	class sort_node_by_pri
	{
		public:
			bool operator()(boost::shared_ptr<Node> node1,
							boost::shared_ptr<Node> node2)
			{
				return (node1->priority() < node2->priority());
			}
	};
};

#endif
