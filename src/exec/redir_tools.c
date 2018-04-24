/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_tools.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jamerlin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/19 15:21:25 by jamerlin          #+#    #+#             */
/*   Updated: 2018/04/21 21:52:31 by videsvau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/header.h"

int		condition_is_valid(t_sh *sh, t_listc *cmd)
{
	if (cmd->prev)
	{
		if (cmd->prev->sep_type & AND)
			if (WEXITSTATUS(sh->retval) != 0 || sh->retval == 127)
				return (0);
		if (cmd->prev->sep_type & OR)
			if (WEXITSTATUS(sh->retval) == 0 && sh->retval != 127)
				return (0);
	}
	return (1);
}

void	prepare_pipe(t_listc *cmd)
{
	t_listc		*cpy;
	int			nb_cmd;
	int			i;

	cpy = cmd;
	nb_cmd = 0;
	i = 0;
	while (cpy->sep_type == PIPE)
	{
		if (cpy->sep_type == PIPE)
			nb_cmd++;
		cpy = cpy->next;
	}
	cpy = cmd;
	nb_cmd++;
	while (cpy && i < nb_cmd)
	{
		cpy->nb_arg = nb_cmd;
		i++;
		cpy = cpy->next;
	}
}

void	closed_unused_fd(int fils, int nb_tube, t_pipe *tabtube)
{
	int			i;

	i = 0;
	while (i < nb_tube)
	{
		if (i != (fils - 1))
			close(tabtube[i].cote[0]);
		if (i != fils)
			close(tabtube[i].cote[1]);
		i++;
	}
}

t_pipe	*new_tabtube(int len)
{
	t_pipe		*ret;
	int			i;

	ret = NULL;
	i = 0;
	if (!(ret = (t_pipe*)malloc(sizeof(t_pipe) * (len + 1))))
		return (NULL);
	while (i < len)
	{
		ret[i].cote[0] = 0;
		ret[i].cote[1] = 0;
		i++;
	}
	return (ret);
}

void	dup_fd(t_listc *cmd, t_pipe *tabtube, int i)
{
	struct stat	stats;

	if (fstat(tabtube[i].cote[0], &stats) == -1)
		g_sh->err = 1;
	else if (cmd->redirs->redir[1] != 0 && cmd->redirs->redir[1] != 4
		&& cmd->redirs->redir[0] == 1)
		dup2(tabtube[i].cote[0], STDOUT_FILENO);
	else if (cmd->redirs->redir[0] == 0 || cmd->redirs->redir[1] == 4)
		dup2(tabtube[i].cote[0], STDIN_FILENO);
	else if (cmd->redirs->redir[0] == 2)
		dup2(tabtube[i].cote[0], STDERR_FILENO);
	else
		dup2(tabtube[i].cote[0], tabtube[i].cote[1]);
	g_sh->err = 0;
}
