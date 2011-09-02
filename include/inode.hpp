#ifndef _INODE_HPP_
#define _INODE_HPP_

#include "math/matrix.hpp"
#include "idriver.hpp"
#include "types.hpp"
#include "transform.hpp"
#include <boost/shared_ptr.hpp>

namespace mhe
{
    // Base renderable class
    class iNode : public Transform
    {
        private:
			int priority_;			

            virtual void draw_impl(const boost::shared_ptr<iDriver>&/*,
                                   const boost::shared_ptr<iCamera>&*/) = 0;
            virtual void update_impl(cmn::uint) {}

            virtual bool is_alive() const {return true;}

			virtual void set_priority(int pri) {priority_ = pri;}
			virtual int get_priority() const   {return priority_;}	// normal priority						
        public:
			iNode() : priority_(2) {}
            virtual ~iNode() {}

            void draw(const boost::shared_ptr<iDriver>& driver/*,
                      const boost::shared_ptr<iCamera>& camera = boost::shared_ptr<iCamera>()*/)
            {
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
    };
	
	// helper classes
	class sort_node_by_pri
	{
		public:
			bool operator()(const boost::shared_ptr<iNode>& node1,
							const boost::shared_ptr<iNode>& node2)
			{
				return (node1->priority() < node2->priority());
			}
	};
};

#endif
