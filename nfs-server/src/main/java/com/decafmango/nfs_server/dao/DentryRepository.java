package com.decafmango.nfs_server.dao;

import java.util.List;
import java.util.Optional;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.stereotype.Repository;

import com.decafmango.nfs_server.model.Dentry;

@Repository
public interface DentryRepository extends JpaRepository<Dentry, Long> {
    
    @Query("select max(d.inode) from Dentry d")
    Integer findMaxInode();

    Optional<Dentry> findByParentInodeAndName(int parentInode, String name);

    List<Dentry> findAllByParentInode(int parentInode);

    Optional<Dentry> findByInode(int inode);

    boolean existsByInode(int inode);

    boolean existsByParentInode(int parentInode);
}
