/*
** EPITECH PROJECT, 2023
** 42sh
** File description:
** sep handler
*/

#include <sys/wait.h>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include "ast.h"
#include "mysh.h"

int execute(ast_t *n, int to_read, int to_write, head_t *head)
{
    int (*ptr)(ast_t *, int, int, head_t *) = NULL;
    int r = 0;

    if (!n || !n->data) return 0;
    if (n->type == SEP) {
        ptr = n->data;
        r = ptr(n, to_read, to_write, head);
    } else {
        dup2(to_read, 0);
        dup2(to_write, 1);
        if (globbings_change_command(((char ***) &(n->data))) == 1) {
            write(2, ((char **)n->data)[0], my_strlen(((char **)n->data)[0]));
            write(2, ": No match.\n", 12);
            return 1;
        }
        r = use_command(n->data, head);
        close(to_read);
        close(to_write);
    }
    return r;
}

int use_semi_colon(ast_t *node, int to_read, int to_write, head_t *head)
{
    execute(node->left, to_read, to_write, head);
    dup2(head->stdin_copy, 0);
    dup2(head->stdout_copy, 1);
    return execute(node->right, to_read, to_write, head);
}

int use_and(ast_t *node, int to_read, int to_write, head_t *head)
{
    int r = 0;
    int save_tr = 0;
    int save_tw = 0;

    save_tr = dup(to_read);
    save_tw = dup(to_write);
    r = execute(node->left, to_read, to_write, head);
    dup2(save_tr, to_read);
    dup2(save_tw, to_write);
    if (r == 0)
        return execute(node->right, to_read, to_write, head);
    return r;
}

int use_or(ast_t *node, int to_read, int to_write, head_t *head)
{
    int r = 0;
    int save_tr = 0;
    int save_tw = 0;

    save_tr = dup(to_read);
    save_tw = dup(to_write);
    r = execute(node->left, to_read, to_write, head);
    dup2(save_tr, to_read);
    dup2(save_tw, to_write);
    if (r != 0)
        return execute(node->right, to_read, to_write, head);
    return r;
}

int use_pipe(ast_t *node, int to_read, int to_write, head_t *head)
{
    int pfd[2] = {0};
    int pid = 0;
    int r = 0;
    int state = 0;

    pipe(pfd);
    pid = fork();
    if (pid < 0) {
        head->keep = false;
        return 84;
    } else if (pid == 0) {
        r = execute(node->left, to_read, pfd[1], head);
        close(pfd[0]);
        exit(r);
    } else {
        close(pfd[1]);
        r = execute(node->right, pfd[0], to_write, head);
        waitpid(pid, &state, 0);
        return r;
    }
}
