#ifndef _HIGHLIGHTED_WIDGET_HPP_
#define _HIGHLIGHTED_WIDGET_HPP_

#include "gui/widget.hpp"

namespace mhe
{
namespace game
{
class HLWidget
{
private:
	typedef gui::PrivateGUIEventHandler<HLWidget> HLWidgetEventHandler;
	friend class gui::PrivateGUIEventHandler<HLWidget>;
	boost::shared_ptr<gui::Widget> widget_;
				
	void init_widget();
				
	void on_mouse_move(const gui::Widget*);
	void on_mouse_left(const gui::Widget*);
public:
	HLWidget() {}
	HLWidget(boost::shared_ptr<gui::Widget> widget);
				
	void setWidget(boost::shared_ptr<gui::Widget> widget);

	boost::shared_ptr<gui::Widget> get() const
	{
		return widget_;
	}
};

}}	// namespaces

#endif
