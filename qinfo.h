#pragma once

struct qinfo_property {
  char *key, *value;
};

struct qinfo_result {
  struct qinfo_property *properties;
  int num_properties;
};

struct qinfo_result qinfo_query(const char *host);

