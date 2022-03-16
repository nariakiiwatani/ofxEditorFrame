#include "ofxEditorFrame.h"
#include "ofGraphics.h"
#include "ofAppRunner.h"
#include "ofEventUtils.h"

ofxEditorFrame::~ofxEditorFrame()
{
	disableMouseInteraction();
}

void ofxEditorFrame::setup()
{
	enableMouseInteraction();
}

void ofxEditorFrame::update()
{
	if(has_unprocessed_mouse_event_) {
		has_unprocessed_mouse_event_ = false;
		ofNotifyEvent(on_mouse_event_, makeArg(), this);
		is_mouse_event_sent_once_ = true;
		mouse_prev_ = mouse_;
	}
}

void ofxEditorFrame::enableMouseInteraction()
{
	if(!is_enabled_mouse_interaction_) {
		auto &ev = ofEvents();
		ofAddListener(ev.mousePressed, this, &ofxEditorFrame::onMousePressed);
		ofAddListener(ev.mouseReleased, this, &ofxEditorFrame::onMouseReleased);
		ofAddListener(ev.mouseMoved, this, &ofxEditorFrame::onMouseMoved);
		ofAddListener(ev.mouseDragged, this, &ofxEditorFrame::onMouseDragged);
		ofAddListener(ev.mouseScrolled, this, &ofxEditorFrame::onMouseScrolled);

		is_enabled_mouse_interaction_ = true;
	}
}
void ofxEditorFrame::disableMouseInteraction()
{
	if(is_enabled_mouse_interaction_) {
		auto &ev = ofEvents();
		ofRemoveListener(ev.mousePressed, this, &ofxEditorFrame::onMousePressed);
		ofRemoveListener(ev.mouseReleased, this, &ofxEditorFrame::onMouseReleased);
		ofRemoveListener(ev.mouseMoved, this, &ofxEditorFrame::onMouseMoved);
		ofRemoveListener(ev.mouseDragged, this, &ofxEditorFrame::onMouseDragged);
		ofRemoveListener(ev.mouseScrolled, this, &ofxEditorFrame::onMouseScrolled);

		is_enabled_mouse_interaction_ = false;
	}
}

void ofxEditorFrame::pushMatrix() const
{
	ofPushMatrix();
	ofTranslate(region_.position);
	ofScale(scale_, scale_);
	ofTranslate(-offset_);
}
void ofxEditorFrame::popMatrix() const
{
	ofPopMatrix();
}
void ofxEditorFrame::pushScissor() const
{
	scissor_cache_.is_enabled = glIsEnabled(GL_SCISSOR_TEST);
	if(scissor_cache_.is_enabled) {
		glGetIntegerv(GL_SCISSOR_BOX, scissor_cache_.box);
	}
	else {
		glEnable(GL_SCISSOR_TEST);
	}
	glScissor(region_.getLeft(), ofGetHeight()-region_.getBottom(), region_.getWidth(), region_.getHeight());
}
void ofxEditorFrame::popScissor() const
{
	if(scissor_cache_.is_enabled) {
		glScissor(scissor_cache_.box[0], scissor_cache_.box[1], scissor_cache_.box[2], scissor_cache_.box[3]);
	}
	else {
		glDisable(GL_SCISSOR_TEST);
	}
}


glm::vec2 ofxEditorFrame::getIn(const glm::vec2 &pos) const
{
	return (pos-glm::vec2(region_.position))/scale_ + offset_;
}
glm::vec2 ofxEditorFrame::getOut(const glm::vec2 &pos) const
{
	return (pos - offset_)*scale_ + glm::vec2(region_.position);
}

void ofxEditorFrame::translate(glm::vec2 on_screen_delta)
{
	offset_ -= on_screen_delta/scale_;
}

void ofxEditorFrame::scale(float mul_scale, glm::vec2 on_screen_anchor)
{
	float scale_prev = scale_;
	scale_ *= mul_scale;
	if(scale_ < settings_.min_scale) {
		scale_ = settings_.min_scale;
	}
	offset_ -= (getIn(on_screen_anchor)-offset_)*(1-scale_/scale_prev);
}

void ofxEditorFrame::resetMatrix()
{
	offset_ = {0,0};
	scale_ = 1;
}

void ofxEditorFrame::onMousePressed(ofMouseEventArgs &arg)
{
	mouse_prev_ = mouse_;
	mouse_pos_pressed_ =
	mouse_prev_.pos =
	mouse_.pos = arg;
	mouse_.button_flag |= (1<<arg.button);
	mouse_.scroll = {0,0};
	has_unprocessed_mouse_event_ = true;
}
void ofxEditorFrame::onMouseReleased(ofMouseEventArgs &arg)
{
//	mouse_.button_flag &= ~(1<<arg.button);
	mouse_.button_flag = 0;
	mouse_.scroll = {0,0};
	has_unprocessed_mouse_event_ = true;
}
void ofxEditorFrame::onMouseMoved(ofMouseEventArgs &arg)
{
	mouse_.pos = arg;
//	mouse_.button_flag = 0;
	mouse_.scroll = {0,0};
	has_unprocessed_mouse_event_ = true;
}
void ofxEditorFrame::onMouseDragged(ofMouseEventArgs &arg)
{
	mouse_.pos = arg;
//	mouse_.button_flag |= (1<<arg.button);
	mouse_.scroll = {0,0};
	has_unprocessed_mouse_event_ = true;
}
void ofxEditorFrame::onMouseScrolled(ofMouseEventArgs &arg)
{
	mouse_.scroll.x = arg.scrollX;
	mouse_.scroll.y = arg.scrollY;
	has_unprocessed_mouse_event_ = true;
}
