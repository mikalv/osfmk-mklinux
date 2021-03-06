/*
 * linux/fs/hfs/dir_cap.c
 *
 * Copyright (C) 1995-1997  Paul H. Hargrove
 * This file may be distributed under the terms of the GNU Public License.
 *
 * This file contains the inode_operations and file_operations
 * structures for HFS directories under the CAP scheme.
 *
 * Based on the minix file system code, (C) 1991, 1992 by Linus Torvalds
 *
 * The source code distribution of the Columbia AppleTalk Package for
 * UNIX, version 6.0, (CAP) was used as a specification of the
 * location and format of files used by CAP's Aufs.  No code from CAP
 * appears in hfs_fs.  hfs_fs is not a work ``derived'' from CAP in
 * the sense of intellectual property law.
 *
 * "XXX" in a comment is a note to myself to consider changing something.
 *
 * In function preconditions the term "valid" applied to a pointer to
 * a structure means that the pointer is non-NULL and the structure it
 * points to has all fields initialized to consistent values.
 */

#include "hfs.h"
#include <linux/hfs_fs_sb.h>
#include <linux/hfs_fs_i.h>
#include <linux/hfs_fs.h>

/*================ Forward declarations ================*/

static int cap_lookup(struct inode *, const char *, int, struct inode **);
static int cap_readdir(struct inode *, struct file *, void *, filldir_t);

/*================ Global variables ================*/

#define DOT_LEN			1
#define DOT_DOT_LEN		2
#define DOT_RESOURCE_LEN	9
#define DOT_FINDERINFO_LEN	11
#define DOT_ROOTINFO_LEN	9

const struct hfs_name hfs_cap_reserved1[] = {
	{DOT_LEN,		"."},
	{DOT_DOT_LEN,		".."},
	{DOT_RESOURCE_LEN,	".resource"},
	{DOT_FINDERINFO_LEN,	".finderinfo"},
	{0,			""},
};

const struct hfs_name hfs_cap_reserved2[] = {
	{DOT_ROOTINFO_LEN,	".rootinfo"},
	{0,			""},
};

#define DOT		(&hfs_cap_reserved1[0])
#define DOT_DOT		(&hfs_cap_reserved1[1])
#define DOT_RESOURCE	(&hfs_cap_reserved1[2])
#define DOT_FINDERINFO	(&hfs_cap_reserved1[3])
#define DOT_ROOTINFO	(&hfs_cap_reserved2[0])

static struct file_operations hfs_cap_dir_operations = {
	NULL,			/* lseek - default */
	hfs_dir_read,		/* read - invalid */
	NULL,			/* write - bad */
	cap_readdir,		/* readdir */
	NULL,			/* select - default */
	NULL,			/* ioctl - default */
	NULL,			/* mmap - none */
	NULL,			/* no special open code */
	NULL,			/* no special release code */
	file_fsync,		/* fsync - default */
	NULL,			/* fasync - default */
	NULL,			/* check_media_change - none */
	NULL			/* revalidate - none */
};

struct inode_operations hfs_cap_ndir_inode_operations = {
	&hfs_cap_dir_operations,/* default directory file-ops */
	hfs_create,		/* create */
	cap_lookup,		/* lookup */
	NULL,			/* link */
	hfs_unlink,		/* unlink */
	NULL,			/* symlink */
	hfs_mkdir,		/* mkdir */
	hfs_rmdir,		/* rmdir */
	hfs_mknod,		/* mknod */
	hfs_rename,		/* rename */
	NULL,			/* readlink */
	NULL,			/* follow_link */
	NULL,			/* readpage */
	NULL,			/* writepage */
	NULL,			/* bmap */
	NULL,			/* truncate */
	NULL,			/* permission */
	NULL			/* smap */
};

struct inode_operations hfs_cap_fdir_inode_operations = {
	&hfs_cap_dir_operations,/* default directory file-ops */
	NULL,			/* create */
	cap_lookup,		/* lookup */
	NULL,			/* link */
	NULL,			/* unlink */
	NULL,			/* symlink */
	NULL,			/* mkdir */
	NULL,			/* rmdir */
	NULL,			/* mknod */
	NULL,			/* rename */
	NULL,			/* readlink */
	NULL,			/* follow_link */
	NULL,			/* readpage */
	NULL,			/* writepage */
	NULL,			/* bmap */
	NULL,			/* truncate */
	NULL,			/* permission */
	NULL			/* smap */
};

struct inode_operations hfs_cap_rdir_inode_operations = {
	&hfs_cap_dir_operations,/* default directory file-ops */
	hfs_create,		/* create */
	cap_lookup,		/* lookup */
	NULL,			/* link */
	NULL,			/* unlink */
	NULL,			/* symlink */
	NULL,			/* mkdir */
	NULL,			/* rmdir */
	NULL,			/* mknod */
	NULL,			/* rename */
	NULL,			/* readlink */
	NULL,			/* follow_link */
	NULL,			/* readpage */
	NULL,			/* writepage */
	NULL,			/* bmap */
	NULL,			/* truncate */
	NULL,			/* permission */
	NULL			/* smap */
};

/*================ File-local functions ================*/

/*
 * cap_lookup()
 *
 * This is the lookup() entry in the inode_operations structure for
 * HFS directories in the CAP scheme.  The purpose is to generate the
 * inode corresponding to an entry in a directory, given the inode for
 * the directory and the name (and its length) of the entry.
 */
static int cap_lookup(struct inode * dir, const char * name,
		      int len, struct inode ** result)
{
	ino_t dtype;
	struct hfs_name cname;
	struct hfs_cat_entry *entry;
	struct hfs_cat_key key;
	struct inode *inode = NULL;
	
	if (!dir || !S_ISDIR(dir->i_mode)) {
		goto done;
	}

	entry = HFS_I(dir)->entry;
	dtype = HFS_ITYPE(dir->i_ino);

	if (len && !name) {
		*result = NULL;
		iput(dir);
		return -EINVAL;
	}

	/* Perform name-mangling */
	hfs_nameout(dir, &cname, name, len);

	/* Check for "." */
	if (hfs_streq(&cname, DOT)) {
		/* this little trick skips the iget and iput */
		*result = dir;
		return 0;
	}

	/* Check for "..". */
	if (hfs_streq(&cname, DOT_DOT)) {
		struct hfs_cat_entry *parent;

		if (dtype != HFS_CAP_NDIR) {
			/* Case for ".." in ".finderinfo" or ".resource" */
			parent = entry;
			++entry->count; /* __hfs_iget() eats one */
		} else {
			/* Case for ".." in a normal directory */
			parent = hfs_cat_parent(entry);
		}
		inode = __hfs_iget(parent, HFS_CAP_NDIR, 0);
		goto done;
	}

	/* Check for special directories if in a normal directory.
	   Note that cap_dupdir() does an iput(dir). */
	if (dtype==HFS_CAP_NDIR) {
		/* Check for ".resource", ".finderinfo" and ".rootinfo" */
		if (hfs_streq(&cname, DOT_RESOURCE)) {
			++entry->count; /* __hfs_iget() eats one */
			inode = __hfs_iget(entry, HFS_CAP_RDIR, 1);
			goto done;
		} else if (hfs_streq(&cname, DOT_FINDERINFO)) {
			++entry->count; /* __hfs_iget() eats one */
			inode = __hfs_iget(entry, HFS_CAP_FDIR, 1);
			goto done;
		} else if ((entry->cnid == htonl(HFS_ROOT_CNID)) &&
			   hfs_streq(&cname, DOT_ROOTINFO)) {
			++entry->count; /* __hfs_iget() eats one */
			inode = __hfs_iget(entry, HFS_CAP_FNDR, 0);
			goto done;
		}
	}

	/* Do an hfs_iget() on the mangled name. */
	hfs_cat_build_key(entry->cnid, &cname, &key);
	inode = hfs_iget(entry->mdb, &key, HFS_I(dir)->file_type);

	/* Don't return a resource fork for a directory */
	if (inode && (dtype == HFS_CAP_RDIR) &&
	    (HFS_I(inode)->entry->type == HFS_CDR_DIR)) {
		iput(inode);
		inode = NULL;
	}

done:
	iput(dir);
	*result = inode;
	return inode ? 0 : -ENOENT;
}

/*
 * cap_readdir()
 *
 * This is the readdir() entry in the file_operations structure for
 * HFS directories in the CAP scheme.  The purpose is to enumerate the
 * entries in a directory, given the inode of the directory and a
 * (struct file *), the 'f_pos' field of which indicates the location
 * in the directory.  The (struct file *) is updated so that the next
 * call with the same 'dir' and 'filp' arguments will produce the next
 * directory entry.  The entries are returned in 'dirent', which is
 * "filled-in" by calling filldir().  This allows the same readdir()
 * function be used for different dirent formats.  We try to read in
 * as many entries as we can before filldir() refuses to take any more.
 *
 * XXX: In the future it may be a good idea to consider not generating
 * metadata files for covered directories since the data doesn't
 * correspond to the mounted directory.	 However this requires an
 * iget() for every directory which could be considered an excessive
 * amount of overhead.	Since the inode for a mount point is always
 * in-core this is another argument for a call to get an inode if it
 * is in-core or NULL if it is not.
 */
static int cap_readdir(struct inode * dir, struct file * filp,
		       void * dirent, filldir_t filldir)
{
	ino_t type;
	int skip_dirs;
	struct hfs_brec brec;
        struct hfs_cat_entry *entry;

	if (!dir || !dir->i_sb || !S_ISDIR(dir->i_mode)) {
		return -EBADF;
	}

        entry = HFS_I(dir)->entry;
	type = HFS_ITYPE(dir->i_ino);
	skip_dirs = (type == HFS_CAP_RDIR);

	if (filp->f_pos == 0) {
		/* Entry 0 is for "." */
		if (filldir(dirent, DOT->Name, DOT_LEN, 0, dir->i_ino)) {
			return 0;
		}
		filp->f_pos = 1;
	}

	if (filp->f_pos == 1) {
		/* Entry 1 is for ".." */
		hfs_u32 cnid;

		if (type == HFS_CAP_NDIR) {
			cnid = hfs_get_nl(entry->key.ParID);
		} else {
			cnid = entry->cnid;
		}

		if (filldir(dirent, DOT_DOT->Name,
			    DOT_DOT_LEN, 1, ntohl(cnid))) {
			return 0;
		}
		filp->f_pos = 2;
	}

	if (filp->f_pos < (dir->i_size - 3)) {
		hfs_u32 cnid;
		hfs_u8 type;

	    	if (hfs_cat_open(entry, &brec) ||
	    	    hfs_cat_next(entry, &brec, filp->f_pos - 2, &cnid, &type)) {
			return 0;
		}
		while (filp->f_pos < (dir->i_size - 3)) {
			if (hfs_cat_next(entry, &brec, 1, &cnid, &type)) {
				return 0;
			}
			if (!skip_dirs || (type != HFS_CDR_DIR)) {
				ino_t ino;
				unsigned int len;
				unsigned char tmp_name[HFS_NAMEMAX];

				ino = ntohl(cnid) | HFS_I(dir)->file_type;
				len = hfs_namein(dir, tmp_name,
				    &((struct hfs_cat_key *)brec.key)->CName);
				if (filldir(dirent, tmp_name, len,
					    filp->f_pos, ino)) {
					hfs_cat_close(entry, &brec);
					return 0;
				}
			}
			++filp->f_pos;
		}
		hfs_cat_close(entry, &brec);
	}

	if (filp->f_pos == (dir->i_size - 3)) {
		if ((entry->cnid == htonl(HFS_ROOT_CNID)) &&
		    (type == HFS_CAP_NDIR)) {
			/* In root dir last-2 entry is for ".rootinfo" */
			if (filldir(dirent, DOT_ROOTINFO->Name,
				    DOT_ROOTINFO_LEN, filp->f_pos,
				    ntohl(entry->cnid) | HFS_CAP_FNDR)) {
				return 0;
			}
		}
		++filp->f_pos;
	}

	if (filp->f_pos == (dir->i_size - 2)) {
		if (type == HFS_CAP_NDIR) {
			/* In normal dirs last-1 entry is for ".finderinfo" */
			if (filldir(dirent, DOT_FINDERINFO->Name,
				    DOT_FINDERINFO_LEN, filp->f_pos,
				    ntohl(entry->cnid) | HFS_CAP_FDIR)) {
				return 0;
			}
		}
		++filp->f_pos;
	}

	if (filp->f_pos == (dir->i_size - 1)) {
		if (type == HFS_CAP_NDIR) {
			/* In normal dirs last entry is for ".resource" */
			if (filldir(dirent, DOT_RESOURCE->Name,
				    DOT_RESOURCE_LEN, filp->f_pos,
				    ntohl(entry->cnid) | HFS_CAP_RDIR)) {
				return 0;
			}
		}
		++filp->f_pos;
	}

	return 0;
}
