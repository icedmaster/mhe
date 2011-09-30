#include "game/highlited_widget.hpp"
#include "utils/sysutils.hpp"

namespace mhe  {
namespace game {

HLWidget::HLWidget(boost::shared_ptr<gui::Widget> widget) :
	widget_(widget)
{
	init_widget();
}
	
void HLWidget::init_widget()
{
	// need to set 2 handlers - for mouse move and for mouse left
	widget_->set_handler(gui::OnMouseMove,
						new HLWidgetEventHandler(this, &HLWidget::on_mouse_move));
	widget_->set_handler(gui::OnMouseLeft,
						new HLWidgetEventHandler(this, &HLWidget::on_mouse_left));
}
	
void HLWidget::setWidget(boost::shared_ptr<gui::Widget> widget)
{
	widget_ = widget;
}
		
void HLWidget::on_mouse_move(const gui::Widget* /*widget*/)
{
	size_t i = widget_->get_sprite()->get_animations_number();
	if (i)
		widget_->get_sprite()->start(i - 1);
}
	
void HLWidget::on_mouse_left(const gui::Widget* /*widget*/)
{
	widget_->get_sprite()->start(0);
}
	
}}
