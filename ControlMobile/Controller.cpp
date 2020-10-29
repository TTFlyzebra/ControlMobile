#include "stdafx.h"
#include "Controller.h"
#define EVENT_STOP (SDL_USEREVENT + 1)

static const int ACTION_DOWN = 1;
static const int ACTION_UP = 1 << 1;

bool IsSocketClosed(SOCKET clientSocket)  
{  
	bool ret = false;  
	HANDLE closeEvent = WSACreateEvent();  
	WSAEventSelect(clientSocket, closeEvent, FD_CLOSE);  
	DWORD dwRet = WaitForSingleObject(closeEvent, 0);
	if(dwRet == WSA_WAIT_EVENT_0)  
		ret = true;  
	else if(dwRet == WSA_WAIT_TIMEOUT)  
		ret = false; 
	WSACloseEvent(closeEvent);  
	return ret;  
} 

static void write_position(uint8_t *buf, const struct position *position) {
    buffer_write32be(&buf[0], position->point.x);
    buffer_write32be(&buf[4], position->point.y);
    buffer_write16be(&buf[8], position->screen_size.width);
    buffer_write16be(&buf[10], position->screen_size.height);
}

size_t utf8_truncation_index(const char *utf8, size_t max_len) {
    size_t len = strlen(utf8);
    if (len <= max_len) {
        return len;
    }
    len = max_len;
    // see UTF-8 encoding <https://en.wikipedia.org/wiki/UTF-8#Description>
    while ((utf8[len] & 0x80) != 0 && (utf8[len] & 0xc0) != 0xc0) {
        // the next byte is not the start of a new UTF-8 codepoint
        // so if we would cut there, the character would be truncated
        len--;
    }
    return len;
}

// write length (2 bytes) + string (non nul-terminated)
static size_t write_string(const char *utf8, size_t max_len, unsigned char *buf) {
    size_t len = utf8_truncation_index(utf8, max_len);
    buffer_write32be(buf, len);
    memcpy(&buf[4], utf8, len);
    return 4 + len;
}

static uint16_t to_fixed_point_16(float f) {
    assert(f >= 0.0f && f <= 1.0f);
    uint32_t u = f * (2^16); 
    if (u >= 0xffff) {
        u = 0xffff;
    }
    return (uint16_t) u;
}

size_t control_msg_serialize(struct control_msg *msg, unsigned char *buf) {
    buf[0] = msg->type;
	size_t ret = 0;
	uint16_t pressure = 0;
    switch (msg->type) {
        case CONTROL_MSG_TYPE_INJECT_KEYCODE:
            buf[1] = msg->inject_keycode.action;
            buffer_write32be(&buf[2], msg->inject_keycode.keycode);
            buffer_write32be(&buf[6], msg->inject_keycode.repeat);
            buffer_write32be(&buf[10], msg->inject_keycode.metastate);
            ret = 14;
			break;
        case CONTROL_MSG_TYPE_INJECT_TEXT: {
            size_t len = write_string(msg->inject_text.text, CONTROL_MSG_INJECT_TEXT_MAX_LENGTH, &buf[1]);
            ret =  1 + len;
			break;
        }
        case CONTROL_MSG_TYPE_INJECT_TOUCH_EVENT:
            buf[1] = msg->inject_touch_event.action;
            buffer_write64be(&buf[2], msg->inject_touch_event.pointer_id);
            write_position(&buf[10], &msg->inject_touch_event.position);
            pressure = to_fixed_point_16(msg->inject_touch_event.pressure);
            buffer_write16be(&buf[22], pressure);
            buffer_write32be(&buf[24], msg->inject_touch_event.buttons);
            ret = 28;
			break;
        case CONTROL_MSG_TYPE_INJECT_SCROLL_EVENT:
            write_position(&buf[1], &msg->inject_scroll_event.position);
            buffer_write32be(&buf[13],
                             (uint32_t) msg->inject_scroll_event.hscroll);
            buffer_write32be(&buf[17],
                             (uint32_t) msg->inject_scroll_event.vscroll);
            ret = 21;
			break;
        case CONTROL_MSG_TYPE_SET_CLIPBOARD: {
            buf[1] = !!msg->set_clipboard.paste;
            size_t len = write_string(msg->set_clipboard.text, CONTROL_MSG_CLIPBOARD_TEXT_MAX_LENGTH, &buf[2]);
            ret = 2 + len;
			break;
        }
        case CONTROL_MSG_TYPE_SET_SCREEN_POWER_MODE:
            buf[1] = msg->set_screen_power_mode.mode;
            ret = 2;
			break;
        case CONTROL_MSG_TYPE_BACK_OR_SCREEN_ON:
        case CONTROL_MSG_TYPE_EXPAND_NOTIFICATION_PANEL:
        case CONTROL_MSG_TYPE_COLLAPSE_NOTIFICATION_PANEL:
        case CONTROL_MSG_TYPE_GET_CLIPBOARD:
        case CONTROL_MSG_TYPE_ROTATE_DEVICE:
            // no additional data
            ret = 1;
			break;
        default:
            ret = 0;
			break;
    }
	return ret;
}

static void send_keycode(struct control_msg *msg, enum android_keycode keycode, int actions, const char *name) {
    // send DOWN event
    msg->type = CONTROL_MSG_TYPE_INJECT_KEYCODE;
    msg->inject_keycode.keycode = keycode;
    msg->inject_keycode.metastate = AMETA_NONE;
    msg->inject_keycode.repeat = 0;

    if (actions & ACTION_DOWN) {
        msg->inject_keycode.action = AKEY_EVENT_ACTION_DOWN;        
    }

    if (actions & ACTION_UP) {
        msg->inject_keycode.action = AKEY_EVENT_ACTION_UP;        
    }
}

#define MAP(FROM, TO) case FROM: *to = TO; return true
#define FAIL default: return false
bool convert_mouse_action(SDL_EventType from, enum android_motionevent_action *to) {
    switch (from) {
        MAP(SDL_MOUSEBUTTONDOWN, AMOTION_EVENT_ACTION_DOWN);
        MAP(SDL_MOUSEBUTTONUP,   AMOTION_EVENT_ACTION_UP);
        FAIL;
    }
}

enum android_motionevent_buttons convert_mouse_buttons(uint32_t state) {
    enum android_motionevent_buttons buttons = AMOTION_EVENT_BUTTON_PRIMARY;
    if (state & SDL_BUTTON_LMASK) {
        buttons = (android_motionevent_buttons)(buttons|AMOTION_EVENT_BUTTON_PRIMARY);
    }
    if (state & SDL_BUTTON_RMASK) {
		buttons = (android_motionevent_buttons)(buttons|AMOTION_EVENT_BUTTON_SECONDARY);
    }
    if (state & SDL_BUTTON_MMASK) {
		buttons = (android_motionevent_buttons)(buttons|AMOTION_EVENT_BUTTON_TERTIARY);
    }
    if (state & SDL_BUTTON_X1MASK) {
		buttons = (android_motionevent_buttons)(buttons|AMOTION_EVENT_BUTTON_BACK);
    }
    if (state & SDL_BUTTON_X2MASK) {
		buttons = (android_motionevent_buttons)(buttons|AMOTION_EVENT_BUTTON_FORWARD);
    }
    return buttons;
}


struct point screen_convert_window_to_frame_coords(struct screen *screen, int32_t x, int32_t y) {
    int ww, wh, dw, dh;
    SDL_GetWindowSize(screen->window, &ww, &wh);
    SDL_GL_GetDrawableSize(screen->window, &dw, &dh);
    // scale for HiDPI (64 bits for intermediate multiplications)
    x = (int64_t) x * dw / ww;
    y = (int64_t) y * dh / wh;
    unsigned rotation = screen->rotation;
    assert(rotation < 4);

    int32_t w = screen->content_size.width;
    int32_t h = screen->content_size.height;


    x = (int64_t) (x - screen->rect.x) * w / screen->rect.w;
    y = (int64_t) (y - screen->rect.y) * h / screen->rect.h;

    struct point result;
    switch (rotation) {
        case 0:
            result.x = x;
            result.y = y;
            break;
        case 1:
            result.x = h - y;
            result.y = x;
            break;
        case 2:
            result.x = w - x;
            result.y = h - y;
            break;
        default:
            assert(rotation == 3);
            result.x = y;
            result.y = w - x;
            break;
    }
    return result;
}

static bool convert_mouse_button(const SDL_MouseButtonEvent *from, struct screen *screen, struct control_msg *to) {
    to->type = CONTROL_MSG_TYPE_INJECT_TOUCH_EVENT;

    if (!convert_mouse_action((SDL_EventType)from->type, &to->inject_touch_event.action)) {
        return false;
    }
	to->inject_touch_event.buttons = convert_mouse_buttons(SDL_BUTTON(from->button));
    to->inject_touch_event.pointer_id = (uint64_t)(-1);
    to->inject_touch_event.position.screen_size = screen->frame_size;
	to->inject_touch_event.position.point.x = from->x*1080/362;
	to->inject_touch_event.position.point.y = from->y*1920/641;
	to->inject_touch_event.pressure = from->type == SDL_MOUSEBUTTONDOWN ? 1.f : 0.f;
    return true;
}

void sendMsg(SOCKET socket, struct control_msg msg){
	static unsigned char serialized_msg[CONTROL_MSG_MAX_SIZE];
	int length = control_msg_serialize(&msg, serialized_msg);
	if (!length) return;
	int w = send(socket,(const char *)serialized_msg,length,0);
}

void input_manager_process_mouse_button(SOCKET socket,  const SDL_MouseButtonEvent *event) {	
    if (event->which == SDL_TOUCH_MOUSEID) {
        return;
    }
	struct control_msg msg;
    bool down = event->type == SDL_MOUSEBUTTONDOWN;
    if (down) {
        if (event->button == SDL_BUTTON_RIGHT) {
            msg.type = CONTROL_MSG_TYPE_BACK_OR_SCREEN_ON;
			sendMsg(socket,msg);
            return;
        }
        if (event->button == SDL_BUTTON_MIDDLE) {
            send_keycode(&msg, AKEYCODE_HOME, ACTION_DOWN | ACTION_UP, "HOME");	
			sendMsg(socket,msg);
            return;
        }

        // double-click on black borders resize to fit the device screen
        if (event->button == SDL_BUTTON_LEFT && event->clicks == 2) {
            //int32_t x = event->x;
            //int32_t y = event->y;
            //screen_hidpi_scale_coords(screen, &x, &y);
            //SDL_Rect *r = &(screen->rect);
            //bool outside = x < r->x || x >= r->x + r->w
            //            || y < r->y || y >= r->y + r->h;
            //if (outside) {
            //    screen_resize_to_fit(im->screen);
            //    return;
            //}
        }
        // otherwise, send the click event to the device
    }

    if (!convert_mouse_button(event, screen, &msg)) {
        return;
    }

	sendMsg(socket,msg);
  
    // Pinch-to-zoom simulation.
    //
    // If Ctrl is hold when the left-click button is pressed, then
    // pinch-to-zoom mode is enabled: on every mouse event until the left-click
    // button is released, an additional "virtual finger" event is generated,
    // having a position inverted through the center of the screen.
    //
    // In other words, the center of the rotation/scaling is the center of the
    // screen.
}

Controller::Controller(void)
{
	socket_lis = INVALID_SOCKET;
	socket_cli = INVALID_SOCKET;
	screen = &flyscreen;
	screen->content_size.width = 360;
	screen->content_size.height = 640;
	screen->frame_size.width = 1080;
	screen->frame_size.height = 1920;
	screen->rotation = 0;
    screen->rect.x = 0;
	screen->rect.y = 0;
	screen->rect.w = 360;
	screen->rect.h = 640;
}

Controller::~Controller(void)
{
}

void Controller::start()
{
	isStop = false;
	m_socketThread = CreateThread(NULL, 0, &Controller::socketThread, this, CREATE_SUSPENDED, NULL);  
	if (NULL!= m_socketThread) {  
		ResumeThread(m_socketThread);  
	}
}

DWORD CALLBACK Controller::socketThread(LPVOID lp)
{
	TRACE("NetWorkService socketThread start. \n");
	Controller *mPtr=(Controller *)lp;
	struct sockaddr_in sin;
	struct sockaddr_in remoteAddr;
	mPtr->socket_lis = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mPtr->socket_lis == INVALID_SOCKET)
	{
		TRACE("socket error !");
		return -1;
	} 		
	sin.sin_family = AF_INET;
	sin.sin_port = htons(9008);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(mPtr->socket_lis, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		TRACE("bind error !");
		return -1;
	}
	if (listen(mPtr->socket_lis, 5) == SOCKET_ERROR)
	{
		TRACE("listen error !");
		return -1;
	}	
	int nAddrlen = sizeof(remoteAddr);
	while (!mPtr->isStop)
	{
		mPtr->socket_cli = accept(mPtr->socket_lis, (SOCKADDR *)&remoteAddr, &nAddrlen);
		TRACE("NetWorkService accept socke_cli=%d.\n",mPtr->socket_cli);
		//if(mPtr->socket_cli != INVALID_SOCKET){
		//	mPtr->m_sendThread = CreateThread(NULL, 0, &Controller::sendThread, &(mPtr->socket_cli), CREATE_SUSPENDED, NULL);  
		//	if (NULL!= mPtr->m_sendThread) {  
		//		ResumeThread(mPtr->m_sendThread);  
		//	}	
		//}
	}	
	TRACE("socketThread exit. \n"); 
	return 0;
}

DWORD CALLBACK Controller::sendThread(LPVOID lp)
{
	SOCKET* pTmp = (SOCKET*)lp;
    SOCKET  m_socket = (SOCKET)(*pTmp);
	SDL_Event event;	
	while (SDL_WaitEvent(&event)) {
		switch (event.type) {
		case EVENT_STOP:
			TRACE("SDLWindow stop\n");
			return 0;
		case SDL_QUIT:
			TRACE("SDL_WaitEvent SDL_QUIT\n");
			return 0;
		case SDL_WINDOWEVENT:
			break;
		case SDL_TEXTINPUT:
			break;
		case SDL_KEYDOWN:
			break;
		case SDL_KEYUP:
			break;
		case SDL_MOUSEMOTION:
			break;
		case SDL_MOUSEWHEEL:
			break;
		case SDL_MOUSEBUTTONDOWN:
			input_manager_process_mouse_button(m_socket,&(event.button));
			TRACE("SDL_MOUSEBUTTONDOWN\n");
			break;
		case SDL_MOUSEBUTTONUP:			
			input_manager_process_mouse_button(m_socket,&(event.button));	
			TRACE("SDL_MOUSEBUTTONUP\n");
			break;
		case SDL_FINGERMOTION:
		case SDL_FINGERDOWN:
		case SDL_FINGERUP:
			break;
		}
		bool ret = IsSocketClosed(m_socket);
		if(ret){
			break;
		}		
	}
	return 0;
}

void Controller::sendMouseEvent(SDL_MouseButtonEvent *button)
{
	if(socket_cli!=INVALID_SOCKET){
		input_manager_process_mouse_button(socket_cli,button);	
	}
}


void Controller::stop()
{
	isStop = true;
	closesocket(socket_cli);
	closesocket(socket_lis);
}