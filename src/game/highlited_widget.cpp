#include "game/highlited_widget.hpp"

namespace mhe  {
namespace game {

HLWidget::HLWidget(boost::shared_ptr<gui::Widget> widget,
				   boost::shared_ptr<iTexture> texture) :
	widget_(widget),
	hl_texture_(texture)
{
	init_widget();
}
	
void HLWidget::init_widget()
{
	// need to set 2 handlers - for mouse move and for mouse left
	widget_->setHandler(gui::OnMouseMove,
						new HLWidgetEventHandler(this, &HLWidget::on_mouse_move));
	widget_->setHandler(gui::OnMouseLeft,
						new HLWidgetEventHandler(this, &HLWidget::on_mouse_left));
}
	
void HLWidget::setWidget(boost::shared_ptr<gui::Widget> widget)
{
	widget_ = widget;
}
	
void HLWidget::setHighlitedTexture(boost::shared_ptr<iTexture> texture)
{
	hl_texture_ = texture;
}
	
void HLWidget::on_mouse_move(const gui::Widget* /*widget*/)
{
	if (!texture_)
		texture_ = widget_->getTexture();
	widget_->setTexture(hl_texture_);
}
	
void HLWidget::on_mouse_left(const gui::Widget* /*widget*/)
{
	widget_->setTexture(texture_);
	texture_.reset();
}
	
}}
