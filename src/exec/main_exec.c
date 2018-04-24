/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_exec.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jamerlin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/19 15:17:47 by jamerlin          #+#    #+#             */
/*   Updated: 2018/04/21 22:08:41 by videsvau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/header.h"

void	close_tabtube(int len, t_pipe *tabtube)
{
	int i;

	i = 0;
	while (i < len)
	{
		if (tabtube[i].cote[0] > 2)
			close(tabtube[i].cote[0]);
		if (tabtube[i].cote[1] > 2)
			close(tabtube[i].cote[1]);
		i++;
	}
	free(tabtube);
}

void	run_cmd(char *fullpath, t_listc *cmd, t_sh *sh, char **env)
{
	if (condition_is_valid(sh, cmd))
	{
		execve(fullpath, cmd->cont, env);
		exit(0);
	}
	else
		exit(WEXITSTATUS(sh->retval));
	print_access_error(fullpath);
	exit(-1);
}

void	fork_exec(char *fullpath, t_listc *cmd, t_pipe *tabtube, t_sh *sh)
{
	char	**env;

	if ((sh->pid = fork()) == 0)
	{
		signal_exec();
		env = env_list_to_char(&sh->env);
		sh->aggr_fail = 1;
		redirect(cmd, tabtube, 0, &cmd->redirs);
		if (sh->aggr_fail)
			(sh->err == 1) ? exit(1) : run_cmd(fullpath, cmd, sh, env);
		else
			exit(1);
	}
	signal(SIGINT, SIG_IGN);
	free(fullpath);
}

void	check_signal(t_sh *sh)
{
	if (WIFSIGNALED(sh->retval) && WTERMSIG(sh->retval) == 2)
		ft_putstr("\n");
}

void	exec_cli(char *cli, t_listc *cmd, t_sh *sh)
{
	char	*fullpath;
	t_pipe	*tabtube;

	sh->pid = 0;
	fullpath = NULL;
	if (!(tabtube = new_tabtube(cmd->nb_arg)))
		return ;
	if (cmd->redirs && cmd->redirs->redir[1] == HEREDOC)
		heredock_redirect(cmd, tabtube, 0);
	if (cmd->sep_type == PIPE)
		sh->retval = init_pipe(cmd, tabtube, sh);
	else if ((fullpath = command_path(&sh->env, cli, sh)))
		fork_exec(fullpath, cmd, tabtube, sh);
	else
	{
		sh->retval = 127;
		return ((void)close_tabtube(cmd->nb_arg, tabtube));
	}
	waitpid(sh->pid, &sh->retval, WUNTRACED);
	signal_init();
	check_signal(sh);
	close_tabtube(cmd->nb_arg, tabtube);
}
