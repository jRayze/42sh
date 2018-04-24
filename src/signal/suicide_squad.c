/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   suicide_squad.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jamerlin <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/19 15:30:25 by jamerlin          #+#    #+#             */
/*   Updated: 2018/04/21 18:30:25 by videsvau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../builtin/builtin.h"
#include "../../inc/header.h"

void		suicide_squad(int signo)
{
	char	*tab[3];

	ft_putstr("Suicide Pid: ");
	ft_putnbr(getpid());
	custom_return();
	signo++;
	tab[0] = "exit";
	tab[1] = "0";
	tab[2] = NULL;
	builtin_exit(tab, g_sh);
}
