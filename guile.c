#include "guile.h"

#include <libguile.h>

#include "common.h"
#include "files.h"
#include "log.h"

void* guile_init_internal(void* init_file) {
  if (NULL == init_file || !can_read_file(init_file)) {
    return NULL;
  }

  scm_c_primitive_load((const char*)init_file);

  return NULL;
}

void guile_init(const char* init_file) {
  scm_with_guile(&guile_init_internal, (void*)init_file);

  logit("Guile initialized");
}

void guile_cleanup() {
}

// Returns a string that must be free()'d or NULL if lyrics file wasn't found.
void* guile_get_lyrics(void* filename) {
  if (scm_to_bool(
          scm_not(scm_defined_p(
              scm_from_utf8_symbol("get-lyrics-file"),
              scm_current_module())))) {
    return NULL;
  }

  SCM lyrics_filename =
      scm_call_1(scm_variable_ref(scm_c_lookup("get-lyrics-file")),
                 scm_from_utf8_string((const char*)filename));

  if(scm_is_string(lyrics_filename)) {
    // TODO: Check if file exists
    return scm_to_utf8_string(lyrics_filename);
  } else {
    return NULL;
  }
}
