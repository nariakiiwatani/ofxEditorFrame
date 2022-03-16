#pragma once

#include "ofRectangle.h"
#include "ofEvents.h"
#include <glm/vec2.hpp>
#include <glm/glm.hpp>

class ofxEditorFrame
{
public:
	~ofxEditorFrame();
	void setup();
	void update();

	void pushMatrix() const;
	void popMatrix() const;
	void pushScissor() const;
	void popScissor() const;
	
	void enableMouseInteraction();
	void disableMouseInteraction();
	void toggleMouseInteractionEnabled() { isMouseInteractionEnabled() ? disableMouseInteraction() : enableMouseInteraction(); }
	bool isMouseInteractionEnabled() const { return is_enabled_mouse_interaction_; }

	void setRegion(const ofRectangle &region) { region_ = region; }
	ofRectangle getRegion() const { return region_; }
	
	void translate(glm::vec2 on_screen_delta);
	void scale(float mul_scale, glm::vec2 on_screen_anchor);
	void resetMatrix();
	
	glm::vec2 getIn(const glm::vec2 &outer_pos) const;
	glm::vec2 getOut(const glm::vec2 &inner_pos) const;
	glm::vec2 getTranslate() const { return offset_; }
	float getScale() const { return scale_; }

	void onMouseEvent(ofMouseEventArgs &arg);
	void onMousePressed(ofMouseEventArgs &arg);
	void onMouseReleased(ofMouseEventArgs &arg);
	void onMouseMoved(ofMouseEventArgs &arg);
	void onMouseDragged(ofMouseEventArgs &arg);
	void onMouseScrolled(ofMouseEventArgs &arg);

	struct MouseEventArg {
		glm::vec2 pos, delta, drag, scroll;
		uint32_t pressed_flag=0, pressing_flag=0, released_flag=0;
		float parent_scale;
		glm::vec2 getPressedPos() const {
			return pos - drag;
		}
		ofRectangle getDragRect() const {
			return {pos-drag, drag.x, drag.y};
		}
		bool isClicked(uint32_t button=OF_MOUSE_BUTTON_LEFT, float threshold=0) const {
			return isReleased(button) && glm::length(drag)<=threshold;
		}
		bool isDragged(uint32_t button=OF_MOUSE_BUTTON_LEFT, float threshold=0) const {
			return isPressing(button) && glm::length(delta)>threshold;
		}
		bool isPressed(uint32_t button=OF_MOUSE_BUTTON_LEFT) const { return (pressed_flag&(1<<button)) != 0; }
		bool isPressing(uint32_t button=OF_MOUSE_BUTTON_LEFT) const { return (pressing_flag&(1<<button)) != 0; }
		bool isReleased(uint32_t button=OF_MOUSE_BUTTON_LEFT) const { return (released_flag&(1<<button)) != 0; }
		bool isPressedAny() const { return pressed_flag != 0; }
		bool isPressingAny() const { return pressing_flag != 0; }
		bool isReleasedAny() const { return released_flag != 0; }
		bool isScrolledX(float threshold=0) const { return glm::abs(scroll.x) > threshold; }
		bool isScrolledY(float threshold=0) const { return glm::abs(scroll.y) > threshold; }
		bool isScrolled(float threshold=0) const { return glm::length(scroll) > threshold; }
	};
	ofEvent<const MouseEventArg> on_mouse_event_;
protected:
	bool is_enabled_mouse_interaction_=false;
	float scale_=1;
	glm::vec2 offset_;
	ofRectangle region_;
	
	struct Settings {
		float min_scale = 0.01f;
	} settings_;
private:
	struct ScissorCache {
		bool is_enabled;
		GLint box[4];
	};
	mutable ScissorCache scissor_cache_;
	struct MouseArg {
		glm::vec2 pos, scroll;
		uint32_t button_flag=0;
	};
	glm::vec2 mouse_pos_pressed_;
	bool has_unprocessed_mouse_event_=false;
	bool is_mouse_event_sent_once_=false;
	MouseArg mouse_, mouse_prev_;
	MouseEventArg makeArg() {
		return makeArg(mouse_, mouse_prev_, mouse_pos_pressed_);
	}
	MouseEventArg makeArg(const MouseArg &current, const MouseArg &prev, const glm::vec2 &pos_pressed) {
		MouseEventArg ret;
		ret.pos = current.pos;
		ret.scroll = current.scroll;
		ret.pressing_flag = current.button_flag;
		if(is_mouse_event_sent_once_) {
			ret.delta = ret.pos - prev.pos;
			ret.drag = ret.pos - pos_pressed;
			ret.pressed_flag = current.button_flag & ~prev.button_flag;
			ret.released_flag = ~current.button_flag & prev.button_flag;
		}
		return ret;
	}
};

