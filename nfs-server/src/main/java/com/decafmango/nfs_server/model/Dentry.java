package com.decafmango.nfs_server.model;

import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.GeneratedValue;
import jakarta.persistence.GenerationType;
import jakarta.persistence.Id;
import jakarta.persistence.Table;
import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;

@Entity
@Table(name = "dentries")
@NoArgsConstructor
@AllArgsConstructor
@Getter
@Setter
public class Dentry {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private long id;

    @Column(name = "inode")
    private int inode;

    @Column(name = "name", length = 256)
    private String name;

    @Column(name = "mode")
    private byte mode;

    @Column(name = "parent_inode")
    private int parentInode;

    @Column(name = "data", length = 2048)
    private String data;
    
}
