#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#ifdef RIGHT
#define BT_ADDR
#endif // RIGHT

#ifdef LEFT
#define BT_ADDR
#endif // LEFT

#ifdef RIGHT
void bt_connect_pc();
void bt_connect_left();
#endif // RIGHT

#ifdef LEFT
void bt_connect_right();
#endif // LEFT

#endif // BLUETOOTH_H
