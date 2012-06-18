#!/bin/bash
#set -v
javac Register.java
jar cfm Register.jar Register.mf Register.class
