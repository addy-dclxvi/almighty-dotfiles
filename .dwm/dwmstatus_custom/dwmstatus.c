/* dwmstatus */
/* icons status wi-fi, BAT, date, time */
#define _DEFAULT_SOURCE
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <X11/Xlib.h>

#define STATUSSZ 256
#define STRSZ 32

typedef enum {
  DOWN,
  UP
} wifi_status_t;

typedef enum {
  NONE,
  UNKNOWN,
  DISCHARGING,
  CHARGING
} battery_status_t;

typedef struct {
  int energy_now;
  int energy_full;
  battery_status_t status;
} battery_info_t;

static int freadint(const char *filename){
  int ret;
  FILE *file = fopen(filename, "r");
  if(!file) {
    fprintf(stderr, "Error opening %s.\n", filename);
    return 0;
  }
  fscanf(file, "%d", &ret);
  fclose(file);
  return ret;
}

static battery_status_t fread_battery_status(const char *filename){
  char buf[16];
  FILE *file = fopen(filename, "r");
  if(!file) {
    fprintf(stderr, "Error opening %s.\n", filename);
    return NONE;
  }
  fgets(buf, 16, file);
  fclose(file);
  if(!strcmp(buf, "Charging\n")) {
    return CHARGING;
  } else if(!strcmp(buf, "Discharging\n")) {
    return DISCHARGING;
  } else {
    return UNKNOWN;
  }
}

static wifi_status_t fread_wifi_status(const char *filename){
  char buf[16];
  FILE *file = fopen(filename, "r");
  if(!file) {
    fprintf(stderr, "Error opening %s.\n", filename);
    return NONE;
  }
  fgets(buf, 16, file);
  fclose(file);
  if(!strcmp(buf, "up\n")) {
    return UP;
  } else {
    return DOWN;
  }
}

static battery_info_t battery_get_info() {
  battery_info_t info;

  info.energy_now = freadint("/sys/class/power_supply/BAT0/energy_now");
  info.energy_full = freadint("/sys/class/power_supply/BAT0/energy_full");
  info.status = fread_battery_status("/sys/class/power_supply/BAT0/status");

  return info;
}

static int battery_get_percent(battery_info_t bat) {
  if(bat.energy_full) {
    return lrint(bat.energy_now * 100.0 / bat.energy_full);
  } else {
    return 0;
  }
}

static int fetch_wifi_info(char *wifi_str) {
  int len = 0;
  wifi_status_t wifi = fread_wifi_status("/sys/class/net/wlp3s0/operstate");

  if(wifi == UP) {
    len += snprintf(wifi_str, STRSZ, "\ue220");
  } else {
    len += snprintf(wifi_str, STRSZ, "\ue222");
  }

  return len;
}

static int fetch_battery_info(char *bat_str) {
  int len = 0;
  battery_info_t bat = battery_get_info();

  int bat_percent = battery_get_percent(bat);

  switch(bat.status) {
    case UNKNOWN :
    case CHARGING :
      len += snprintf(bat_str, STRSZ, "\ue23a");
      break;
    case DISCHARGING :
      	if(bat_percent > 56) {
        len += snprintf(bat_str, STRSZ, "\ue238");
      } else if(bat_percent > 24) {
        len += snprintf(bat_str, STRSZ, "\ue237");
      } else if(bat_percent > 10) {
        len += snprintf(bat_str, STRSZ, "\ue239");
      } else if(bat_percent > 2) {
	len += snprintf(bat_str, STRSZ, "\ue0a7");
      }
      break;
    default:
      len += snprintf(bat_str, STRSZ, "\ue238");
  }

  len += snprintf(bat_str + len, STRSZ - len, "", bat_percent);

  return len;
}

static int fetch_current_time(char *time_str) {
  time_t current_time = time(NULL);
  struct tm *current_tm = localtime(&current_time);

  return strftime(time_str, STRSZ, "%a %d, %b %H:%M %p \ue0a5 ", current_tm);
}

int main(void) {
  static Display *dpy;
  char status[STATUSSZ];
  char battery[STRSZ];
  char wifi[STRSZ];
  char time[STRSZ];

  if (!(dpy = XOpenDisplay(NULL))) {
    fprintf(stderr, "dwmstatus: cannot open display.\n");
    return 1;
  }

  for (;;sleep(20)) {
    fetch_wifi_info(wifi);
    fetch_battery_info(battery);
    fetch_current_time(time);

    snprintf(status, STATUSSZ, "%s %s %s", wifi, battery, time);

    XStoreName(dpy, DefaultRootWindow(dpy), status);
    XFlush(dpy);
  }

  XCloseDisplay(dpy);

  return 0;
}
/* the end */
