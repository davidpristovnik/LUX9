#!/bin/bash

# Original script done by Don Darling
# Later changes by Koen Kooi and Brijesh Singh
# Changes for Evo-Teh Lux boards by David Pristovnik

# Revision history:
# 20100602 First release
#

###############################################################################
# OE_BASE    - The root directory for all OE sources and development.
###############################################################################
OE_BASE=${PWD}
OE_ENV=./.oe/environment
#OE_ENV=~/.oe/environment

###############################################################################
# SET_ENVIRONMENT() - Setup environment variables for OE development
###############################################################################
function set_environment()
{

#--------------------------------------------------------------------------
# If an env already exists, use it, otherwise generate it
#--------------------------------------------------------------------------
if [ -e ${OE_ENV} ] ; then
    echo "Using ${OE_ENV} to setup needed variables. "
    echo "You can do '. ${OE_ENV}' and run 'bitbake something' without using $0 as wrapper"
    . ${OE_ENV}
else

    mkdir -p ./.oe/

    #--------------------------------------------------------------------------
    # Specify distribution information
    #--------------------------------------------------------------------------
    DISTRO="angstrom-2008.1"
    DISTRO_DIRNAME=`echo $DISTRO | sed s#[.-]#_#g`

    echo "export DISTRO=\"${DISTRO}\"" > ${OE_ENV}
    echo "export DISTRO_DIRNAME=\"${DISTRO_DIRNAME}\"" >> ${OE_ENV}

    #--------------------------------------------------------------------------
    # Specify the root directory for your OpenEmbedded development
    #--------------------------------------------------------------------------
    OE_BUILD_DIR=${OE_BASE}/lux
    OE_BUILD_TMPDIR="${OE_BUILD_DIR}/tmp-${DISTRO_DIRNAME}"
    OE_SOURCE_DIR=${OE_BUILD_DIR}/sources
    OE_RECIPES_DIR=${OE_BUILD_DIR}/recipes

    mkdir -p ${OE_BUILD_DIR}
    mkdir -p ${OE_SOURCE_DIR}
    mkdir -p ${OE_RECIPES_DIR}
    export OE_BASE

    echo "export OE_BUILD_DIR=\"${OE_BUILD_DIR}\"" >> ${OE_ENV}
    echo "export OE_BUILD_TMPDIR=\"${OE_BUILD_TMPDIR}\"" >> ${OE_ENV}
    echo "export OE_SOURCE_DIR=\"${OE_SOURCE_DIR}\"" >> ${OE_ENV}

    echo "export OE_BASE=\"${OE_BASE}\"" >> ${OE_ENV}

    #--------------------------------------------------------------------------
    # Include up-to-date bitbake in our PATH.
    #--------------------------------------------------------------------------
    export PATH=${OE_BASE}/bitbake/bin:${PATH}

    echo "export PATH=\"${PATH}\"" >> ${OE_ENV}

    #--------------------------------------------------------------------------
    # Make sure Bitbake doesn't filter out the following variables from our
    # environment.
    #--------------------------------------------------------------------------
    export BB_ENV_EXTRAWHITE="MACHINE DISTRO GIT_PROXY_COMMAND"

    echo "export BB_ENV_EXTRAWHITE=\"${BB_ENV_EXTRAWHITE}\"" >> ${OE_ENV}

    #--------------------------------------------------------------------------
    # Set up the bitbake path to find the OpenEmbedded recipes.
    #--------------------------------------------------------------------------
    export BBPATH=${OE_BUILD_DIR}:${OE_BASE}/openembedded${BBPATH_EXTRA}
  
    echo "export BBPATH=\"${BBPATH}\"" >> ${OE_ENV}
 
    #--------------------------------------------------------------------------
    # Reconfigure dash 
    #--------------------------------------------------------------------------
    if [ $(readlink /bin/sh) = "dash" ] ; then
        sudo dpkg-reconfigure dash
    fi

    echo "There now is a sourceable script in ${OE_ENV} ."
    echo "You can do '. ${OE_ENV}' and run 'bitbake something' without using $0 as wrapper"
fi # if -e ~/.oe/environment
}


###############################################################################
# UPDATE_ALL() - Make sure everything is up to date
###############################################################################
function update_all()
{
    set_environment
    update_bitbake
    update_oe
}

###############################################################################
# CLEAN_OE() - Delete TMPDIR
###############################################################################
function clean_oe()
{
    set_environment
	echo "Cleaning ${OE_BUILD_TMPDIR}"
	rm -rf ${OE_BUILD_TMPDIR}
}


###############################################################################
# OE_BUILD() - Build an OE package or image
###############################################################################
function oe_build()
{
    set_environment
    cd ${OE_BUILD_DIR}
    if [ -z $MACHINE ] ; then
        echo "Executing: bitbake" $*
        bitbake $*
    else
        echo "Executing: MACHINE=${MACHINE} bitbake" $*
        MACHINE=${MACHINE} bitbake $*
    fi
}


###############################################################################
# OE_CONFIG() - Configure OE for a target 
###############################################################################
function oe_config()
{
    set_environment
    config_oe

    echo ""
    echo "Setup for ${CL_MACHINE} completed"
}


###############################################################################
# UPDATE_BITBAKE() - Update Bitbake distribution
###############################################################################
function update_bitbake()
{
	if [ ! -d ${OE_BASE}/bitbake/bin ]; then
            rm -rf  ${OE_BASE}/bitbake
            echo Checking out bitbake
            git clone git://git.openembedded.org/bitbake ${OE_BASE}/bitbake
            cd ${OE_BASE}/bitbake && git checkout -b 1.10 origin/1.10
        else
            cd ${OE_BASE}/bitbake && git pull --rebase
        fi
}


###############################################################################
# UPDATE_OE() - Update OpenEmbedded distribution.
###############################################################################
function update_oe()
{
        if [ ! -d  ${OE_BASE}/openembedded/conf ]; then
            rm -rf  ${OE_BASE}/openembedded/
            echo Checking out OpenEmbedded
            git clone "git://git.openembedded.org/openembedded" ${OE_BASE}/openembedded
            cd ${OE_BASE}/openembedded
            if [ ! -r ${OE_COMMIT_ID} ]; 
            then
                echo "Checkout commit id: ${OE_COMMIT_ID}"
                git checkout -b install ${OE_COMMIT_ID}
            else
                git checkout -b org.openembedded.dev origin/org.openembedded.dev
            fi
        else
            echo Updating OpenEmbedded
            cd ${OE_BASE}/openembedded
            git pull --rebase 
        fi
}


###############################################################################
# CONFIG_OE() - Configure OpenEmbedded
###############################################################################
function config_oe()
{
    #--------------------------------------------------------------------------
    # Determine the proper machine name
    #--------------------------------------------------------------------------
    case ${CL_MACHINE} in
        at91sam9260ek)
            MACHINE="at91sam9260ek"
            ;;
        at91sam9263ek)
            MACHINE="at91sam9263ek"
            ;;
        lux-sfx9|sfx9)
            MACHINE="lux-sfx9"
            ;;
        lux-sf9|sf9)
            MACHINE="lux-sf9"
            ;;
        *)
            echo "Unknown machine ${CL_MACHINE}, passing it to OE directly"
            MACHINE="${CL_MACHINE}"
            ;;
    esac

    #--------------------------------------------------------------------------
    # Write out the OE bitbake configuration file.
    #--------------------------------------------------------------------------
    mkdir -p ${OE_BUILD_DIR}/conf

    # There's no need to rewrite local.conf when changing MACHINE
    if [ ! -e ${OE_BUILD_DIR}/conf/local.conf ]; then
        cat > ${OE_BUILD_DIR}/conf/local.conf <<_EOF
# Where to store sources
DL_DIR = "${OE_BUILD_DIR}/downloads"

INHERIT += "rm_work"

# Which files do we want to parse:
BBFILES := "${OE_BASE}/openembedded/recipes/*/*.bb"
BBMASK = ""

# Qemu 0.12.x is giving too much problems recently (2010.05), so disable it for users
ENABLE_BINARY_LOCALE_GENERATION = "0"

# What kind of images do we want?
IMAGE_FSTYPES += "tar.bz2 jffs2"

# Make use of SMP:
#   PARALLEL_MAKE specifies how many concurrent compiler threads are spawned per bitbake process
#   BB_NUMBER_THREADS specifies how many concurrent bitbake tasks will be run
PARALLEL_MAKE     = "-j2"
BB_NUMBER_THREADS = "2"

DISTRO   = "${DISTRO}"
MACHINE ?= "${MACHINE}"

# Set TMPDIR instead of defaulting it to $pwd/tmp
TMPDIR = "${OE_BUILD_TMPDIR}"

# Don't generate the mirror tarball for SCM repos, the snapshot is enough
BB_GENERATE_MIRROR_TARBALLS = "0"

_EOF
fi
}


###############################################################################
# Build the specified OE packages or images.
###############################################################################

if [ $# -gt 0 ]; then
    case $1 in
        update)
            shift
            if [ ! -r $1 ]; then
                case $1 in
                    commit)
                        shift
                        OE_COMMIT_ID=$1
                        ;;
                esac
            fi
            update_all
            exit 0
            ;;
        bitbake)
            shift
            oe_build $*
            exit 0
            ;;
        config)
            shift
            CL_MACHINE=$1
            shift
            oe_config $*
            exit 0
            ;;
        clean)
            clean_oe
            exit 0
            ;;
    esac
fi

#then
#    if [ $1 = "update" ]
#    then
#        shift
#        if [ ! -r $1 ]; then
#            if [  $1 == "commit" ]
#            then
#                shift
#                OE_COMMIT_ID=$1
#            fi
#        fi
#        update_all
#        exit 0
#    fi
#
#    if [ $1 = "bitbake" ]
#    then
#        shift
#        oe_build $*
#        exit 0
#    fi
#
#    if [ $1 = "config" ]
#    then
#        shift
#        CL_MACHINE=$1
#        shift
#        oe_config $*
#        exit 0
#    fi
#
#    if [ $1 = "clean" ]
#    then
#        clean_oe
#        exit 0
#    fi
#fi

# Help Screen
echo ""
echo "Usage: $0 config <machine>"
echo "       $0 update"
echo "       $0 bitbake <bitbake target>"
echo "       $0 clean"
echo ""
echo "You must invoke \"$0 config <machine>\" and then \"$0 update\" prior"
echo "to your first bitbake command"
echo ""
echo "The <machine> argument can be one of the following"
echo "       lux-sf9:           Lux SF9 board"
echo "       lux-sfx9:          Lux SFX9 board"
echo "       at91sam9260ek:     Atmel AT91SAM9260EK board"
echo "       at91sam9263ek:     Atmel AT91SAM9263EK board"
echo ""
echo "Other machines are valid as well, but listing those would make this message way too long"
