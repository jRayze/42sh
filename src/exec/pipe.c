/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jamerlin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/19 15:17:54 by jamerlin          #+#    #+#             */
/*   Updated: 2018/04/19 15:17:55 by jamerlin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/header.h"

void	ft_cmd_pipe(t_listc *cmd, t_sh *sh)
{
	char	*fullpath;
	int		(*func)(char **, t_sh*);

	if (cmd->func)
	{
		func = cmd->func;
		sh->retval = func(cmd->cont, sh);
		exit(sh->retval);
	}
	else if (!(fullpath = command_path(&sh->env, cmd->cont[0], sh)))
		exit(1);
	execve(fullpath, cmd->cont, NULL);
	errexit("Execve failed.");
	exit(1);
}

void	pipe_tmp(t_listc *cmd, int i, t_pipe *tabtube, t_sh *sh)
{
	closed_unused_fd(i, cmd->nb_arg - 1, tabtube);
	if (i > 0)
	{
		close(STDIN_FILENO);
		dup2(tabtube[i - 1].cote[0], STDIN_FILENO);
	}
	if (i < (cmd->nb_arg - 1))
	{
		close(STDOUT_FILENO);
		dup2(tabtube[i].cote[1], STDOUT_FILENO);
	}
	if (cmd->redirs && cmd->redirs->redir[1] == 4)
		heredock_redirect(cmd, tabtube, i);
	sh->aggr_fail = 1;
	redirect(cmd, tabtube, i, &cmd->redirs);
	if (sh->aggr_fail)
		ft_cmd_pipe(cmd, sh);
	else
		exit(1);
}

void	ft_pipe(t_listc *cmd, int *pid_tab, t_pipe *tabtube, t_sh *sh)
{
	pid_t son;

	son = -1;
	son = fork();
	if (son == -1)
	{
		close(tabtube[sh->i].cote[0]);
		close(tabtube[sh->i].cote[1]);
		errexit("Fork failed.");
		exit(1);
	}
	else if (son == 0)
		pipe_tmp(cmd, sh->i, tabtube, sh);
	pid_tab[sh->i] = son;
}

int		do_pipe(t_listc *cmd, int *pid_tab, t_pipe *tabtube, t_sh *sh)
{
	int		wt_cpt;
	t_listc	*cpy;

	sh->i = 0;
	wt_cpt = -1;
	cpy = cmd;
	while (sh->i < cmd->nb_arg - 1)
	{
		ft_pipe(cmd, pid_tab, tabtube, sh);
		cmd = cmd->next;
		sh->i++;
	}
	signal(SIGINT, SIG_IGN);
	ft_pipe(cmd, pid_tab, tabtube, sh);
	closed_unused_fd(-1, cmd->nb_arg - 1, tabtube);
	while (++wt_cpt < cmd->nb_arg)
	{
		waitpid(pid_tab[wt_cpt], &cpy->status, 0);
		sh->retval = cpy->status;
		cpy = cpy->next;
	}
	free(pid_tab);
	return (sh->retval);
}

int		init_pipe(t_listc *cmd, t_pipe *tabtube, t_sh *sh)
{
	int		*pid_tab;
	int		i;

	i = 0;
	if (!(pid_tab = (int *)malloc(sizeof(int) * (cmd->nb_arg + 1))))
		exit(-1);
	while (i < (cmd->nb_arg - 1))
	{
		if (pipe(tabtube[i].cote) != 0)
		{
			errexit("Pipe failed.");
			free(tabtube);
			exit(-2);
		}
		i++;
	}
	return (do_pipe(cmd, pid_tab, tabtube, sh));
}
