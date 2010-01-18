/** @file
 * @brief HID report descriptor - option library test
 *
 * Copyright (C) 2010 Nikolai Kondrashov
 *
 * This file is part of hidrd.
 *
 * Hidrd is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Hidrd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with hidrd; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * @author Nikolai Kondrashov <spbnick@gmail.com>
 *
 * @(#) $Id$
 */


#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include "hidrd/opt/spec_list.h"
#include "hidrd/opt/list.h"

int
main(int argc, char **argv)
{
    const char             *orig_spec_str;
    char                   *orig_spec_buf   = NULL;
    hidrd_opt_spec         *test_spec_list  = NULL;
    char                   *test_spec_str   = NULL;
    const char             *orig_opt_str;
    char                   *orig_opt_buf    = NULL;
    hidrd_opt              *test_opt_list   = NULL;
    char                   *test_opt_str    = NULL;

    (void)argc;
    (void)argv;

    /*******************************************
     * Test option specification list parsing
     *******************************************/
    orig_spec_str = "format=b?yes'Format XML output,"
                    "author=s?Nikolai Kondrashov'Descriptor author,"
                    "group=b?yes'Group paired items into single elements,"
                    "type=s'Type,"
                    "obscure=b";

    /* Parse specification list */
    orig_spec_buf = strdup(orig_spec_str);

    test_spec_list = hidrd_opt_spec_list_parse(orig_spec_buf);
    if (test_spec_list == NULL)
        error(1, errno, "Failed to parse option specification list");

    if (hidrd_opt_spec_list_len(test_spec_list) != 5)
        error(1, 0, "Option specification list length is %u, instead of %u",
              hidrd_opt_spec_list_len(test_spec_list), 5);

#define CHECK(_i, _name, _T, _t, _req, _dflt, _desc) \
    do {                                                                \
        const hidrd_opt_spec   *_s = &test_spec_list[_i];               \
                                                                        \
        if (strcmp(_s->name, _name) != 0)                               \
            error(1, 0,                                                 \
                  "Option specification #%u name is \"%s\" "            \
                  "instead of \"%s\"", _i, _s->name, _name);            \
        if (_s->type != HIDRD_OPT_TYPE_##_T)                            \
            error(1, 0,                                                 \
                  "Option specification #%u type is '%c' "              \
                  "instead of '%c'", _i, _s->type,                      \
                  HIDRD_OPT_TYPE_##_T);                                 \
        if (_s->req != _req)                                            \
            error(1, 0,                                                 \
                  "Option specification #%u \"required\" flag is %s "   \
                  "instead of %s", _i,                                  \
                  (_s->req ? "true" : "false"),                         \
                  (_req ? "true" : "false"));                           \
        if (!_req && hidrd_opt_type_cmp_##_t(_s->dflt._t, _dflt) != 0)  \
        {                                                               \
            char    *_test_str;                                         \
            char    *_orig_str;                                         \
                                                                        \
            _test_str = hidrd_opt_type_format_##_t(_s->dflt._t);        \
            _orig_str = hidrd_opt_type_format_##_t(_dflt);              \
                                                                        \
            error(1, 0,                                                 \
                  "Option specification #%u default value is \"%s\" "   \
                  "instead of \"%s\"", _i, _test_str, _orig_str);       \
        }                                                               \
        if (strcmp(_s->desc, _desc) != 0)                               \
            error(1, 0,                                                 \
                  "Option specification #%u description is \"%s\" "     \
                  "instead of \"%s\"", _i, _s->desc, _desc);            \
    } while (0)

    /* Check parsed data */
    CHECK(0, "format", BOOLEAN, boolean, false, true,
          "Format XML output");
    CHECK(1, "author", STRING, string, false, "Nikolai Kondrashov",
          "Descriptor author");
    CHECK(2, "group", BOOLEAN, boolean, false, true,
          "Group paired items into single elements");
    CHECK(3, "type", STRING, string, true, "", "Type");
    CHECK(4, "obscure", BOOLEAN, boolean, true, false, "");

#undef CHECK

    /* Format specification list */
    test_spec_str = hidrd_opt_spec_list_format(test_spec_list);
    if (test_spec_str == NULL)
        error(1, errno, "Failed to format option specification list");

    if (strcmp(test_spec_str, orig_spec_str) != 0)
        error(1, 0,
              "Test specification list string:\n%s\n"
              "mismatches the original:\n%s\n",
              test_spec_str, orig_spec_str);

    free(test_spec_str);

    /*******************************************
     * Test option list parsing
     *******************************************/
    orig_opt_str = "format=yes,group=no,type=test,obscure=no";
    orig_opt_buf = strdup(orig_opt_str);

    test_opt_list = hidrd_opt_spec_list_apply_parse(test_spec_list, orig_opt_buf);
    if (test_opt_list == NULL)
        error(1, errno, "Failed to parse option list");

    if (hidrd_opt_list_len(test_opt_list) != 5)
        error(1, 0, "Unexpected option list length: %u instead of %u",
              hidrd_opt_list_len(test_opt_list), 5);

    test_opt_str = hidrd_opt_list_format(test_opt_list);
    if (test_opt_str == NULL)
        error(1, errno, "Failed to format option list");

    fprintf(stderr, "test_opt_str: \"%s\"\n", test_opt_str);

    free(test_opt_str);

    free(test_opt_list);
    free(orig_opt_buf);

    free(test_spec_list);
    free(orig_spec_buf);

    return 0;
}
