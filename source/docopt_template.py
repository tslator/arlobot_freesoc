#!/usr/bin/env python
#-*- coding:utf-8 -*-


# Copyright (c) 2012 Vladimir Keleshev, <vladimir@keleshev.com>
# (see LICENSE-MIT file for copying)


"""Usage: docopt_c.py [options] [<docopt>]

Processes a docopt formatted string, from either stdin or a file, and
outputs the equivalent C code to parse a CLI, to either the stdout or a file.

Options:
  -o, --output-name=<outname>
                Filename used to write the produced C file.
                If not present, the produced code is printed to stdout.
  -m, --module=<module>
                Filename of the C module.
  -h,--help     Show this help message and exit.

Arguments:
  <docopt>      Input file describing your CLI in docopt language.

"""


import sys
import os.path
import re
import docopt
from string import Template
import textwrap
import numbers


def to_c(s):
    if type(s) is str:
        # Note: docopt requires a program name, e.g console, but the Psoc console will not require it
        #       so we are removing 'console' from the help and 
        # Note: The Psoc serial port requires \r\n for newline
        return ('"%s"' % s.replace('\\', r'\\')\
                          .replace('"', r'\"')\
                          .replace('\n', '\\r\\n"\n"')\
                          .replace('console ', r''))
    if s is True:
        return '1'
    if s is False:
        return '0'
    if isinstance(s, numbers.Number):
        return str(s)
    if s is None:
        return 'NULL'
    raise ValueError("can't convert to c type: %r" % s)


def c_command(o):
    return '{%s}' % ', '.join(to_c(v) for v in (o.name, o.value))


def c_argument(o):
    return '{%s}' % ', '.join(to_c(v) for v in (o.name, o.value, None))


def c_option(o):
    return '{%s}' % ', '.join(to_c(v) for v in (o.short, o.long, o.argcount,
                                                False, None))


def c_name(s):
    return ''.join(c if c.isalnum() else '_' for c in s).strip('_')


def c_if_command(cmd):
    t = """if (!strcmp(command->name, %s)) {
            args->%s = command->value;
        }"""
    return t % (to_c(cmd.name), c_name(cmd.name))


def c_if_argument(arg):
    t = """if (!strcmp(argument->name, %s)) {
            args->%s = argument->value;
        }"""
    return t % (to_c(arg.name), c_name(arg.name))


def c_if_flag(o):
    t = """ else if (!strcmp(option->o%s, %s)) {
            args->%s = option->value;
        }"""
    return t % (('long' if o.long else 'short'),
                to_c(o.long or o.short),
                c_name(o.long or o.short))


def c_if_option(o):
    t = """ else if (!strcmp(option->o%s, %s)) {
            if (option->argument)
                args->%s = option->argument;
        }"""
    return t % (('long' if o.long else 'short'),
                to_c(o.long or o.short),
                c_name(o.long or o.short))

def extract_usage(command, text):
    test = '    console {} '.format(command)
    start = 0
    end = len(text)
    not_done = True;
    command_list = []
    
    while not_done:
        result = text.find(test, start, end)
        if (result > 0):
            newline = text.find('\n', result)
            command_list.append(text[result:newline])
            start = result + len(test)
        else:
            not_done = False
            
    return command_list
    
def create_command_usage(command, usage_text):
    usage_list = extract_usage(command, usage_text)
    usage_output = '{} Usage\n'.format(command.title()) + 'Usage:\n'
    usage_output += '\n'.join(usage_list)
    
    return usage_output
    
def create_command_help(command, usage_text, options_text):
    usage_list = extract_usage(command, usage_text)
    options_list = options_text[1:]
    matching_option_items = []
    help_output = ''
    
    for usage_item in usage_list:
        usage_params = usage_item.split(' ')[6:]

        for param in usage_params:
            if param.startswith('['):
                param = param[1:]
            if param.endswith(']'):
                param = param[:-1]
            if param.startswith('('):
                param = param[1:]
            if param.endswith(')'):
                param = param[:-1]
            if not param.startswith('--'):
                continue
                
            for option_item in options_list:
                if param in option_item:
                    matching_option_items.append(option_item)
                    
    help_output = '{} Help\n'.format(command.title())
    help_output += 'Usage:\n'
    help_output += '\n'.join(usage_list)
    help_output += '\n\n'
    help_output += 'Options:\n'
    help_output += '\n'.join(set(matching_option_items))
    
    return help_output
                
def parse_leafs(pattern, all_options):
    options_shortcut = False
    leafs = []
    queue = [(0, pattern)]
    while queue:
        level, node = queue.pop(-1)  # depth-first search
        if not options_shortcut and type(node) == docopt.OptionsShortcut:
            options_shortcut = True
        elif hasattr(node, 'children'):
            children = [((level + 1), child) for child in node.children]
            children.reverse()
            queue.extend(children)
        else:
            if node not in leafs:
                leafs.append(node)
    sort_by_name = lambda e: e.name
    leafs.sort(key=sort_by_name)
    commands = [leaf for leaf in leafs if type(leaf) == docopt.Command]
    arguments = [leaf for leaf in leafs if type(leaf) == docopt.Argument]
    if options_shortcut:
        option_leafs = all_options
        option_leafs.sort(key=sort_by_name)
    else:
        option_leafs = [leaf for leaf in leafs if type(leaf) == docopt.Option]
    flags = [leaf for leaf in option_leafs if leaf.argcount == 0]
    options = [leaf for leaf in option_leafs if  leaf.argcount > 0]
    leafs = [i for sl in [commands, arguments, flags, options] for i in sl]
    return leafs, commands, arguments, flags, options


if __name__ == '__main__':
    args = docopt.docopt(__doc__)

    try:
        if args['<docopt>'] is not None:
            with open(args['<docopt>'], 'r') as f:
                args['<docopt>'] = f.read()
        elif args['<docopt>'] is None and sys.stdin.isatty():
            print(__doc__.strip("\n"))
            sys.exit("")
        else:
            args['<docopt>'] = sys.stdin.read()
        if args['--module'] is None:
            args['--module'] = os.path.join(
                    os.path.dirname(os.path.realpath(__file__)), "template.c")
        #with open(args['--module'], 'r') as f:
        #        args['--module'] = f.read()
    except IOError as e:
        sys.exit(e)

    doc = args['<docopt>']
    usage = docopt.parse_section('usage:', doc)
    s = ['More than one ', '"usage:" (case-insensitive)', ' not found.']
    usage = {0: s[1:], 1: usage[0] if usage else None}.get(len(usage), s[:2])
    if isinstance(usage, list):
        raise docopt.DocoptLanguageError(''.join(usage))

    all_options = docopt.parse_defaults(doc)
    pattern = docopt.parse_pattern(docopt.formal_usage(usage), all_options)
    leafs, commands, arguments, flags, options = parse_leafs(pattern, all_options)

    t_commands = ';\n    '.join('int %s' % c_name(cmd.name)
                                for cmd in commands)
    t_commands = (('\n    /* commands */\n    ' + t_commands + ';')
                  if t_commands != '' else '')
    t_arguments = ';\n    '.join('char *%s' % c_name(arg.name)
                                 for arg in arguments)
    t_arguments = (('\n    /* arguments */\n    ' + t_arguments + ';')
                   if t_arguments != '' else '')
    t_flags = ';\n    '.join('int %s' % c_name(flag.long or flag.short)
                             for flag in flags)
    t_flags = (('\n    /* options without arguments */\n    ' + t_flags + ';')
               if t_flags != '' else '')
    t_options = ';\n    '.join('char *%s' % c_name(opt.long or opt.short)
                               for opt in options)
    t_options = (('\n    /* options with arguments */\n    ' + t_options + ';')
                 if t_options != '' else '')
    t_defaults = ', '.join(to_c(leaf.value) for leaf in leafs)
    t_defaults = re.sub(r'"(.*?)"', r'(char*) "\1"', t_defaults)
    t_defaults = '\n        '.join(textwrap.wrap(t_defaults, 72))
    t_defaults = ('\n        ' + t_defaults + ',') if t_defaults != '' else ''
    t_elems_cmds = ',\n        '.join([c_command(cmd) for cmd in (commands)])
    t_elems_cmds = ('\n        ' + t_elems_cmds) if t_elems_cmds != '' else ''
    t_elems_args = ',\n        '.join([c_argument(arg) for arg in (arguments)])
    t_elems_args = ('\n        ' + t_elems_args) if t_elems_args != '' else ''
    t_elems_opts = ',\n        '.join([c_option(o) for o in (flags + options)])
    t_elems_opts = ('\n        ' + t_elems_opts) if t_elems_opts != '' else ''
    t_elems_n = ', '.join([str(len(l))
                           for l in [commands, arguments, (flags + options)]])
    t_if_command = ' else '.join(c_if_command(command) for command in commands)
    t_if_command = ('\n        ' + t_if_command) if t_if_command != '' else ''
    t_if_argument = ' else '.join(c_if_argument(arg) for arg in arguments)
    t_if_argument = (('\n        ' + t_if_argument)
                     if t_if_argument != '' else '')
    t_if_flag = ''.join(c_if_flag(flag) for flag in flags)
    t_if_option = ''.join(c_if_option(opt) for opt in options)

    # We will create two files: <module>.h and <module>.c using ['--module']+.h and ['--module'].c
    
    # Create the h file
    template_h = ""
    with open(args['--module']+'.h', 'r') as f:
        template_h = f.read()

    module_h = Template(template_h).safe_substitute(
        module=args['--output-name'].upper()+'_H',
        commands=t_commands,
        arguments=t_arguments,
        flags=t_flags,
        options=t_options 
    )        
    
    # Create the c file
    template_c = ""
    with open(args['--module']+'.c', 'r') as f:
        template_c = f.read()

    help_options = docopt.parse_section('options:', doc)[0].split('\n')[1:]
        
    module_c = Template(template_c).safe_substitute(
            module=args['--output-name'],
            commands=t_commands,
            arguments=t_arguments,
            flags=t_flags,
            options=t_options,
            help_message=to_c(doc),
            usage_pattern=to_c(usage),
            motor_help_message=to_c(create_command_help('motor', usage, help_options)),
            motor_usage_pattern=to_c(create_command_usage('motor', usage)),
            pid_help_message=to_c(create_command_help('pid', usage, help_options)),
            pid_usage_pattern=to_c(create_command_usage('pid', usage)),
            config_help_message=to_c(create_command_help('config', usage, help_options)),
            config_usage_pattern=to_c(create_command_usage('config', usage)),
            motion_help_message=to_c(create_command_help('motion', usage, help_options)),
            motion_usage_pattern=to_c(create_command_usage('motion', usage)),
            if_flag=t_if_flag,
            if_option=t_if_option,
            if_command=t_if_command,
            if_argument=t_if_argument,
            defaults=t_defaults,
            elems_cmds=t_elems_cmds,
            elems_args=t_elems_args,
            elems_opts=t_elems_opts,
            elems_n=t_elems_n)

    if args['--output-name'] is None:
        print("Module Header")
        print(module_h.strip() + '\n')
        print("Module C")
        print(module_c.strip() + '\n')
    else:
        try:
            with open(args['--output-name']+'.h', 'w') as f:
                f.write(module_h.strip() + '\n')
            with open(args['--output-name']+'.c', 'w') as f:
                f.write(module_c.strip() + '\n')
        except IOError as e:
            sys.exit(str(e))
