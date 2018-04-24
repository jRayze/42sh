/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_event.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jamerlin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/19 15:29:18 by jamerlin          #+#    #+#             */
/*   Updated: 2018/04/21 21:44:22 by vboivin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/header.h"
#include "../builtin/builtin.h"

void		free_closing(t_sh *sh, t_close **close)
{
	t_close	*cp;
	t_close *tmp;

	cp = (*close);
	while (g_sh->inpl && g_sh->inpl->previous)
	{
		free_list_from_beginning(&g_sh->inpl->inp);
		g_sh->inpl = g_sh->inpl->previous;
		free(g_sh->inpl->next);
		g_sh->inpl->next = NULL;
	}
	free_list_from_beginning(&g_sh->inpl->inp);
	while (cp)
	{
		tmp = cp;
		cp = cp->next;
		free(tmp);
	}
	sh->close = NULL;
}

void		signal_print_prompt(int inp)
{
	if (!g_sh->ctrl_c)
		return ;
	(void)inp;
	custom_return();
	g_sh->retval = 1;
	print_prompt(g_sh);
	free_comp(11, g_sh);
	g_sh->context = 0;
	if (g_sh->search)
	{
		free_list_from_beginning(&g_sh->inpl->inp);
		g_sh->search = 0;
		ft_putstr(tgetstr("ve", NULL));
	}
	else if (!g_sh->inpl->previous && !g_sh->close)
		free_list_from_beginning(&g_sh->inpl->inp);
	else
		free_closing(g_sh, &g_sh->close);
}

void		signal_init_two_because_norm_lol(void)
{
	signal(SIGTTOU, SIG_IGN);
	signal(SIGXCPU, SIG_IGN);
	signal(SIGPROF, SIG_IGN);
	signal(SIGUSR1, SIG_IGN);
}

void		signal_init(void)
{
	char	*tab[4];

	if (g_sh->retval == 9)
	{
		tab[0] = "/usr/bin/killall";
		tab[1] = "-1";
		tab[2] = "42sh";
		tab[3] = NULL;
		execve(tab[0], tab, NULL);
		exit(9);
	}
	signal(SIGINT, &signal_print_prompt);
	signal(SIGHUP, &suicide_squad);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGILL, SIG_IGN);
	signal(SIGTRAP, SIG_IGN);
	signal(SIGABRT, SIG_IGN);
	signal(SIGKILL, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGCONT, SIG_IGN);
	signal(SIGXFSZ, SIG_IGN);
	signal(SIGVTALRM, SIG_IGN);
	signal_init_two_because_norm_lol();
}

void		signal_exec(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGKILL, SIG_DFL);
	signal(SIGHUP, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGILL, SIG_DFL);
	signal(SIGTRAP, SIG_DFL);
	signal(SIGABRT, SIG_DFL);
	signal(SIGKILL, SIG_DFL);
	signal(SIGTERM, SIG_DFL);
	signal(SIGTSTP, SIG_DFL);
	signal(25, SIG_DFL);
	signal(26, SIG_DFL);
	signal(27, SIG_DFL);
	signal(30, SIG_DFL);
}
