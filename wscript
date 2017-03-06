#! /usr/bin/env python
# encoding: utf-8

import os
import waflib.extras.wurf_options

APPNAME = 'score-multicast-redirection'
VERSION = '0.0.0'


def options(opt):

    opt.load('wurf_common_tools')

def resolve(ctx):

    import waflib.extras.wurf_dependency_resolve as resolve

    ctx.load('wurf_common_tools')

    ctx.add_dependency(resolve.ResolveVersion(
        name='waf-tools',
        git_repository='github.com/steinwurf/waf-tools.git',
        major=3))

    ctx.add_dependency(resolve.ResolveVersion(
        name='score-cpp',
        git_repository='gitlab.com/steinwurf/score-cpp.git',
        major=4))

    ctx.add_dependency(resolve.ResolveVersion(
        name='links',
        git_repository='gitlab.com/steinwurf/links.git',
        major=7))

def configure(conf):

    conf.load("wurf_common_tools")


def build(bld):

    bld.load("wurf_common_tools")

    bld.env.append_unique(
        'DEFINES_STEINWURF_VERSION',
        'STEINWURF_SCORE_MULTICAST_REDIRECTION_VERSION="{}"'.format(VERSION))

    if bld.is_toplevel():

        # Only build tests when executed from the top-level wscript,
        # i.e. not when included as a dependency
        bld.recurse('src/score-multicast-redirection')
