/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jamerlin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/19 15:11:45 by jamerlin          #+#    #+#             */
/*   Updated: 2018/04/21 12:12:58 by drecours         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/header.h"
#include "builtin.h"

int			print_env_tab(char **tab)
{
	int		i;

	i = 0;
	while (tab[i])
	{
		ft_putendl_fd(tab[i], STDOUT_FILENO);
		i++;
	}
	env_free(tab);
	return (0);
}

char		**env_in_tab(t_env **env)
{
	int		i;
	t_env	*cp;
	char	**tab;

	cp = *env;
	i = 0;
	while (cp)
	{
		i++;
		cp = cp->next;
	}
	if (!(tab = (char**)malloc(sizeof(char*) * (i + 1))))
		return (NULL);
	tab[i] = NULL;
	cp = *env;
	i = 0;
	while (cp)
	{
		if (cp->env)
			tab[i] = ft_strdup(cp->env);
		i++;
		cp = cp->next;
	}
	return (tab);
}

int			exec_cmd(char **tab, char **exec, t_sh *sh, t_env **env)
{
	int		i;
	char	*path;
	int		(*func)(char**, t_sh *);
	char	*tmp;

	i = 127;
	func = NULL;
	if ((func = get_builtin_function(exec[0])))
	{
		free_list(&sh->env);
		return (sh->retval = func(exec, sh));
	}
	if ((path = command_path(env, exec[0], sh)))
	{
		if ((tmp = check_path_bin(path, sh)))
		{
			i = fork_command(path, exec, tab);
			free(tmp);
		}
		else
			i = 127;
		free(path);
	}
	free_list(&sh->env);
	return (i);
}

void		free_list(t_env **env)
{
	t_env	*cp;
	t_env	*tmp;

	tmp = NULL;
	if ((cp = (*env)))
	{
		while (cp)
		{
			if (cp->env)
				free(cp->env);
			tmp = cp;
			cp = cp->next;
			free(tmp);
		}
		(*env) = NULL;
	}
}

int			builtin_env(char **exec, t_sh *sh)
{
	char	**tab;
	int		i;
	int		verbose;
	t_env	*new_env;

	i = 0;
	verbose = 0;
	if (!exec[1] || too_big(exec))
		return (print_env(&sh->env));
	if (!(tab = env_in_tab(&sh->env)))
		return (1);
	if (!(flag_v_u_i(&tab, exec, &verbose)))
		return (env_free(tab));
	tab = flag_i(tab, exec, verbose);
	if (!(i = flag_equal(&tab, exec, verbose)))
		return (2);
	if (!exec[i])
		return (print_env_tab(tab));
	new_env = sh->env;
	sh->env = tab_in_env(tab);
	flag_v(exec, &(exec[i]));
	i = exec_cmd(tab, &(exec[i]), sh, &new_env);
	env_free(tab);
	sh->env = new_env;
	return (i);
}
