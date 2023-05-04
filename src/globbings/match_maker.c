/*
** EPITECH PROJECT, 2023
** 42sh
** File description:
** Build the list of matches for a globbing pattern
*/

#include <stdlib.h>
#include <string.h>
#include "mystr.h"
#include "globber.h"

static list_t *get_all_matches(const char *pattern)
{
    char **pparsed = NULL;
    char *prefix = NULL;

    if (!pattern) {
        return NULL;
    }
    if (pattern[0] == '/') {
        prefix = strdup("/");
    } else {
        prefix = strdup("");
    }
    pparsed = parse_pattern(pattern);
    return reslist_from_ppattern(pparsed, prefix, 0);
}

static list_t *spread_matches(list_t *results)
{
    char *tmp = NULL;
    char **tab = NULL;
    list_t *res = NULL;

    res = list_init();
    while (results->size > 0) {
        tmp = results->interface->popf(results);
        if (!tmp)
            continue;
        tab = my_str_to_word_array(tmp, ' ');
        for (int i = 0; tab[i]; i++) {
            res->interface->append(res, strdup(tab[i]));
        }
        free(tmp);
        free(tab);
    }
    return res;
}

int globbings_change_command(char ***commands)
{
    list_t *tmp = NULL;

    for (int i = 0; (*commands)[i]; i++) {
        if (!is_valid_pattern((*commands)[i])) {
            continue;
        }
        tmp = get_all_matches((*commands)[i]);
        if (!tmp || tmp->size == 0) {
            return 1;
        }
        tmp = spread_matches(tmp);
        sort_matches(tmp->head);
        move_dirs_to_end(tmp);
        insert_list_in_tab(commands, tmp, i);
    }
    return 0;
}
