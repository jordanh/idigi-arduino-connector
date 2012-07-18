/*! @page  rebuild_srpm Rebuilding the iDigi Package (SRPM)
 * 
 * @htmlinclude nav.html
 *
 * @section rebuild_srpm_overview Overview
 * 
 *  The iDigi Connector library is provided in the form of a source RPM (SRPM) which is included
 *  in the Wind River Linux distribution.  If required you can modify and rebuild the iDigi library
 *  this is currently only required when modifying RCI.
 * 
 * @section modifying_code Modifying Code in the SRPM
 * 
 *  The Wind River workbench extracts the SRPM into the directory: /home/wruser/WindRiver/workspace/WRLinux-Platform_prj/build/idigi-1.1.
 *  The source code for the package is contained in the /home/wruser/WindRiver/workspace/WRLinux-Platform_prj/build/idigi-1.1/BUILD
 *.  Be careful to backup any modifications made to this directory; this directory is overwritten if the package is reimported.
 * 
 * 
 *  @note If you distclean the package the /home/wruser/WindRiver/workspace/WRLinux-Platform_prj/build/idigi-1.1. is deleted.
 *  Backup any modifications you make in this directory, they are not added to the SRPM only into the build directory.
 *
 * 
 * @section rebuild_srpm_details How to rebuild the iDigi SRPM
 * 
 *  You can view the idigi package in the Wind River Workbench
 *  by clicking on WRLinux-Platform and then selecting User Space Configuration, this is shown in the
 *  screen below:
 * 
 * @image html workbench1.png
 * 
 * You can rebuild the iDigi SRPM by clicking on the @b rebuild button which is circled in the above picture.
 * This will create a new RPM which will be built in the directory /WindRiver/workspace/WRLinux-Platform_prj/export/RPMS/atom.
 * The new RPM name will have the idigi -1.1-1.atom-[md5 sum].rpm, the MD5 sum will change when the project is
 * rebuilt.
 * 
 * After building the new RPM copy it to the device (using scp) and reinstall using the syntax:
 * 
 * An example copy command is:
 * 
 *      @htmlonly scp idigi-debuginfo-1.1-1.atom-2ee4548.rpm root@172.31.255.1:/  @endhtmlonly
 * 
 * This will copy to the root directory to the device which has the IP address 172.31.255.1.
 * 
 * The command to reinstall the RPM on the device is:
 * 
 *     rpm -i -v --force idigi-1.1-1.atom-2ee4548.rpm
 * 
 * Note that the name of the RPM contains the MD5 sum of the RPM, this will change when the package is rebuilt.
 *
 * @htmlinclude terminate.html
 */

