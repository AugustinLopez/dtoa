/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dtoa.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aulopez <aulopez@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/28 17:41:59 by aulopez           #+#    #+#             */
/*   Updated: 2020/11/28 18:30:37 by aulopez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define EXPONENT_MAX 1023
#define MANTIS_SIZE 52
#define BIGINT_SIZE 300
#define DECIMAL_START 150

/*
** ------ SECTION BIGBUFF ------------------------------------------------------
*/

typedef	struct s_bigbuff
{
	uint16_t	start;
	uint16_t	end;
	uint32_t	val[BIGINT_SIZE];
}				t_bigbuff;

/*
** Function for debugging
*/

void	bigbuff_print(bool is_negative, t_bigbuff buff)
{
	int32_t	i;
	int32_t j;
	int32_t	k;

	if (buff.start > buff.end)
		return;
	if (is_negative)
		printf("-");
	i = buff.start;
	if (i > DECIMAL_START)
	{
		j = buff.start;
		printf("0.");
		while (j-- > DECIMAL_START)
			printf("000000000");
	}
	i -= 1;
	while (++i <= buff.end)
	{
		if (i == DECIMAL_START)
		{
			if (i == buff.start)
				printf("0.");
			else
				printf(".");
		}
		if (i == buff.start && i < DECIMAL_START)
			printf("%d", buff.val[i]);
		else if (i == buff.end && i >= DECIMAL_START)
		{
			j = 9;
			k = 1;
			while (buff.val[i] % (k * 10) == 0)
			{
				k *= 10;
				j--;
			}
			printf("%0*d", j, buff.val[i] / k);
		}
		else
			printf("%09d",(buff.val)[i]);
	}
	printf("\n");
}

t_bigbuff	bigbuff_multiply_by_2_pow(t_bigbuff src, int16_t exponent)
{
	uint16_t	i;

	while (exponent--)
	{
		i = src.start;
		while (i <= src.end)
		{
			(src.val)[i] *= 2;
			if ((src.val)[i] >= 1000000000)
			{
				(src.val)[i] -= 1000000000;
				(src.val)[i - 1] += 1;
				if (i == src.start)
					--(src.start);
			}
			if (i == src.end && (src.val)[i] == 0)
				--(src.end);
			++i;
		}
	}
	return (src);
}

t_bigbuff	bigbuff_divided_by_2_pow(t_bigbuff src, int16_t exponent)
{
	uint16_t	i;

	while (exponent++)
	{
		i = src.end;
		while (i >= src.start)
		{
			if ((src.val)[i] & 1)
			{
				(src.val)[i + 1] += 500000000;
				if (i == src.end)
					++(src.end);
			}
			(src.val)[i] /= 2;
			if (i == src.start && (src.val)[i] == 0)
				++(src.start);
			--i;
		}
	}
	return (src);
}

void	bigbuff_add_right_to_left(t_bigbuff *left, t_bigbuff right)
{
	uint16_t	start;
	uint16_t	end;
	uint16_t	i;

	start = left->start < right.start ? left->start : right.start;
	end = left->end > right.end ? left->end : right.end;
	i = end;
	while (i >= start)
	{
		(left->val)[i] += (right.val)[i];
		if ((left->val)[i] >= 1000000000)
		{
			(left->val)[i] -= 1000000000;
			(left->val)[i - 1] += 1;
		}
		--i;
		if (i < start && (left->val)[i] != 0)
			--start;
	}
	left->end = end;
	left->start = start;
}

t_bigbuff	bigbuff_init_zero(void)
{
	t_bigbuff	ret;

	memset(&ret, 0, sizeof(ret));
	ret.start = BIGINT_SIZE - 1;
	ret.end = 0;
	return (ret);
}

t_bigbuff	bigbuff_init_unit(int16_t exponent)
{
	t_bigbuff	ret;

	memset(&ret, 0, sizeof(ret));
	ret.val[DECIMAL_START - 1] = 1;
	ret.start = DECIMAL_START - 1;
	ret.end = DECIMAL_START - 1;
	if (exponent < 0)
		ret = bigbuff_divided_by_2_pow(ret, exponent);
	else if (exponent > 0)
		ret = bigbuff_multiply_by_2_pow(ret, exponent);
	return (ret);
}

/*
** ----- SECTION STRING --------------------------------------------------------
*/

char	*itoa(int n, char *ret)
{
	uint16_t			len;
	int				buf;
	unsigned int	positif;

	len = (n < 0) + 1;
	buf = n;
	while ((buf /= 10))
		++len;
	ret[len] = 0;
	if (n < 0)
	{
		ret[0] = '-';
		buf = 1;
	}
	positif = n * (1 - 2 * (n < 0));
	while (len > (unsigned int)buf)
	{
		ret[--len] = '0' + positif % 10;
		positif /= 10;
	}
	return (ret);
}

uint16_t	d_init(char	*ret, bool is_negative, t_bigbuff buff)
{
	uint16_t	i;
	uint16_t	index;

	index = 0;
	if (is_negative)
	{
		strcat(ret, "-");
		index = 1;
	}
	if (buff.start >= DECIMAL_START)
	{
		i = buff.start;
		strcat(ret + index, "0");
		index += 1;
		if (buff.start > DECIMAL_START)
		{
			strcat(ret + index, ".");
			index += 1;
		}
		while (i-- > DECIMAL_START)
		{
			strcat(ret + index, "000000000");
			index += 9;
		}
	}
	return (index);
}

uint16_t	d_load_dot(char *ret, uint16_t index, t_bigbuff buff)
{
	if (index > buff.start || (buff.start != buff.end))
	{
		strcat(ret + index, ".");
		++index;
	}
	return (index);
}

uint16_t	d_load_number(char *ret, t_bigbuff buff, uint16_t i, uint16_t index)
{
	char	for_itoa[10];
	uint32_t	k;
	uint16_t	j;

	if (i == buff.start && i < DECIMAL_START)
	{
		itoa(buff.val[i], for_itoa);
		strcat(ret + index, for_itoa);
		index += strlen(for_itoa);
	}
	else if (i == buff.end && i >= DECIMAL_START)
	{
		k = 100000000;
		while (buff.val[i] / k == 0)
		{
			ret[index++] = '0';
			k /= 10;
		}
		k = 1;
		while (buff.val[i] % (k * 10) == 0)
			k *= 10;
		itoa(buff.val[i]/k, for_itoa);
		strcat(ret + index, for_itoa);
		index += strlen(for_itoa);
	}
	else
	{
		itoa(buff.val[i], for_itoa);
		k = strlen(for_itoa);
		j = k;
		while (j++ < 9)
			ret[index++] = '0';
		strcat(ret + index, for_itoa);
		index += k;
	}
	return (index);
}

char	*d_string(bool is_negative, t_bigbuff buff)
{
	char	*ret;
	uint16_t	index;
	uint16_t	i;

	//'+ 1 + 1' for potential decimal separator and minus sign
	ret = malloc((buff.end - buff.start + 1) * 9 + 1 + 1);
	if (ret == NULL)
		return (NULL);
	index = d_init(ret, is_negative, buff);
	i = buff.start - 1;
	while (++i <= buff.end)
	{
		if (i == DECIMAL_START)
			index = d_load_dot(ret, index, buff);
		index = d_load_number(ret, buff, i, index);
	}
	return (ret);
}
char	*d_is_nan(bool is_negative, uint64_t mantissa)
{
	char	buf[4];

	if (mantissa >> 63 != 0)
		mantissa = 0;
	if (is_negative && mantissa == 0)
		strcat(buf, "-inf");
	else if (mantissa == 0)
		strcat(buf, "inf");
	else
		strcat(buf, "NaN");
	return (strdup(buf));
}

char	*d_is_zero(bool is_negative)
{
	if (is_negative)
		return (strdup("-0"));
	return (strdup("0"));
}

char	*dtoa(double x)
{
	uint64_t	mantissa;
	bool		is_negative;
	int16_t		exponent;
	t_bigbuff	unit;
	t_bigbuff	result;
	int16_t		i;

	memcpy(&mantissa, &x, sizeof(uint64_t));
	exponent = ((mantissa << 1) >> 53) - EXPONENT_MAX;
	is_negative = (mantissa >> 63);
	mantissa = ((mantissa << 12) >> 12);
	if (x == 0)
		return (d_is_zero(is_negative));
	if (exponent >= EXPONENT_MAX + 1)
		return (d_is_nan(is_negative, mantissa));
	unit = bigbuff_init_unit(exponent - MANTIS_SIZE);
	//bigbuff_print(false, unit);
	result = bigbuff_init_zero();
	i = -1;
	while (++i < MANTIS_SIZE)
	{
		if ((mantissa >> i) & 1)
			bigbuff_add_right_to_left(&result, unit);
		unit = bigbuff_multiply_by_2_pow(unit, 1);
		//bigbuff_print(false, unit);
	}
	bigbuff_add_right_to_left(&result, unit);
	//bigbuff_print(is_negative, result);
	return (d_string(is_negative, result));
}

/*
** Provided for testing
*/

int		main(int ac, char **av)
{
	double	x;
	char	*dummy;
	char	*s;

	if (ac < 2)
		return (-1);
	x = strtod(av[1], &dummy);
	printf("Printf conversion: %.100f\n", x);
	s = dtoa(x);
	printf("Dtoa conversion:   %s\n", s);
	free(s);
	return (0);
}
