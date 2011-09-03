#ifndef _HIGHLIGHTED_WIDGET_HPP_
#define _HIGHLIGHTED_WIDGET_HPP_

#include "gui/widget.hpp"
#include "itexture.hpp"

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
	boost::shared_ptr<iTexture> texture_;
	boost::shared_ptr<iTexture> hl_texture_;
				
	void init_widget();
				
	void on_mouse_move(const gui::Widget*);
	void on_mouse_left(const gui::Widget*);
public:
	HLWidget() {}	
	HLWidget(boost::shared_ptr<gui::Widget> widget,
			 boost::shared_ptr<iTexture> texture);
				
	void setWidget(boost::shared_ptr<gui::Widget> widget);
	void setHighlitedTexture(boost::shared_ptr<iTexture> texture);

	boost::shared_ptr<gui::Widget> get() const
	{
		return widget_;
	}
};

}}	// namespaces

#endif
