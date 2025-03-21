#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <xcb/xcb.h>

#define WIDTH 300
#define HEIGHT 100

void
print_modifiers (uint32_t mask)
{
    const char *MODIFIERS[] = {
            "Shift", "Lock", "Ctrl", "Alt",
            "Mod2", "Mod3", "Mod4", "Mod5",
            "Button1", "Button2", "Button3", "Button4", "Button5"
    };

    printf ("Modifier mask: ");
    for (const char **modifier = MODIFIERS ; mask; mask >>= 1, ++modifier) {
        if (mask & 1) {
            printf (*modifier);
        }
    }
    printf ("\n");
}

void
event_log (xcb_generic_event_t *event)
{
    switch (event->response_type & ~0x80) {
    case XCB_EXPOSE: {
        xcb_expose_event_t *expose = (xcb_expose_event_t *)event;
    
        printf ("Window %"PRIu32" exposed. Region to be redrawn at location (%"PRIu16",%"PRIu16"), with dimension (%"PRIu16",%"PRIu16")\n",
                expose->window, expose->x, expose->y, expose->width, expose->height );
        break;
    }
    case XCB_BUTTON_PRESS: {
        xcb_button_press_event_t *bp = (xcb_button_press_event_t *)event;
        print_modifiers (bp->state);
    
        switch (bp->detail) {
        case 4:
            printf ("Wheel Button up in window %"PRIu32", at coordinates (%"PRIi16",%"PRIi16")\n",
                    bp->event, bp->event_x, bp->event_y );
            break;
        case 5:
            printf ("Wheel Button down in window %"PRIu32", at coordinates (%"PRIi16",%"PRIi16")\n",
                    bp->event, bp->event_x, bp->event_y );
            break;
        default:
            printf ("Button %"PRIu8" pressed in window %"PRIu32", at coordinates (%"PRIi16",%"PRIi16")\n",
                    bp->detail, bp->event, bp->event_x, bp->event_y );
            break;
        }
        break;
    }
    case XCB_BUTTON_RELEASE: {
        xcb_button_release_event_t *br = (xcb_button_release_event_t *)event;
        print_modifiers(br->state);
    
        printf ("Button %"PRIu8" released in window %"PRIu32", at coordinates (%"PRIi16",%"PRIi16")\n",
                br->detail, br->event, br->event_x, br->event_y );
        break;
    }
    case XCB_MOTION_NOTIFY: {
        xcb_motion_notify_event_t *motion = (xcb_motion_notify_event_t *)event;
    
        printf ("Mouse moved in window %"PRIu32", at coordinates (%"PRIi16",%"PRIi16")\n",
                motion->event, motion->event_x, motion->event_y );
        break;
    }
    case XCB_ENTER_NOTIFY: {
        xcb_enter_notify_event_t *enter = (xcb_enter_notify_event_t *)event;
    
        printf ("Mouse entered window %"PRIu32", at coordinates (%"PRIi16",%"PRIi16")\n",
                enter->event, enter->event_x, enter->event_y );
        break;
    }
    case XCB_LEAVE_NOTIFY: {
        xcb_leave_notify_event_t *leave = (xcb_leave_notify_event_t *)event;
    
        printf ("Mouse left window %"PRIu32", at coordinates (%"PRIi16",%"PRIi16")\n",
                leave->event, leave->event_x, leave->event_y );
        break;
    }
    case XCB_KEY_PRESS: {
        xcb_key_press_event_t *kp = (xcb_key_press_event_t *)event;
        print_modifiers(kp->state);
    
        printf ("Key pressed in window %"PRIu32"\n",
                kp->event);
        break;
    }
    case XCB_KEY_RELEASE: {
        xcb_key_release_event_t *kr = (xcb_key_release_event_t *)event;
        print_modifiers(kr->state);
    
        printf ("Key released in window %"PRIu32"\n",
                kr->event);
        break;
    }
    default:
        /* Unknown event type, ignore it */
        printf ("Unknown event: %"PRIu8"\n",
                event->response_type);
        break;
    }
}

int
main ()
{
    int screenNum;
    xcb_connection_t *connection = xcb_connect(NULL, &screenNum);
    
    if (!connection) {
      fprintf(stderr, "ERROR: can't connect to an X server\n");
      return -1;
    }
    
    xcb_screen_iterator_t iter =
        xcb_setup_roots_iterator(xcb_get_setup(connection));

    // we want the screen at index screenNum of the iterator
    for (int i = 0; i < screenNum; ++i) {
      xcb_screen_next(&iter);
    }

    xcb_screen_t *screen = iter.data;

    if (!screen) {
      fprintf(stderr, "ERROR: can't get the current screen\n");
      xcb_disconnect(connection);
      return -1;
    }

    xcb_window_t window    = xcb_generate_id (connection);

    uint32_t     values[2] = {screen->black_pixel,
                                XCB_EVENT_MASK_EXPOSURE       | XCB_EVENT_MASK_BUTTON_PRESS   |
                                XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
                                XCB_EVENT_MASK_ENTER_WINDOW   | XCB_EVENT_MASK_LEAVE_WINDOW   |
                                XCB_EVENT_MASK_KEY_PRESS      | XCB_EVENT_MASK_KEY_RELEASE };

      char *title = "Xlog";
      xcb_change_property (connection,
                           XCB_PROP_MODE_REPLACE,
                           window,
                           XCB_ATOM_WM_NAME,
                           XCB_ATOM_STRING,
                           8,
                           strlen (title),
                           title );

    xcb_map_window (connection, window);
    xcb_flush (connection);

    xcb_generic_event_t *event;
    while ( (event = xcb_wait_for_event (connection)) ) {
        event_log(event);   
        free (event);
        }
    return 0;
}
