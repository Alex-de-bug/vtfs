package com.example.demo.models;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import jakarta.persistence.CascadeType;
import jakarta.persistence.Entity;
import jakarta.persistence.GeneratedValue;
import jakarta.persistence.GenerationType;
import jakarta.persistence.Id;
import jakarta.persistence.JoinColumn;
import jakarta.persistence.ManyToOne;
import jakarta.persistence.OneToMany;
import jakarta.persistence.Table;
import lombok.Data;

@Entity
@Data
@Table(name = "directories")
public class DirectoryEntity {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private UUID id;

    private String name;

    @ManyToOne
    @JoinColumn(name = "parent_id")
    private DirectoryEntity parent;

    @OneToMany(mappedBy = "parent", cascade = CascadeType.ALL)
    private List<FileEntity> files = new ArrayList<>();

    @OneToMany(mappedBy = "parent", cascade = CascadeType.ALL)
    private List<DirectoryEntity> subdirectories = new ArrayList<>();
}
