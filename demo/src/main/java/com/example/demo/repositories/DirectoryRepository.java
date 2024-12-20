package com.example.demo.repositories;

import java.util.List;
import java.util.UUID;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import com.example.demo.models.DirectoryEntity;

@Repository
public interface DirectoryRepository extends JpaRepository<DirectoryEntity, UUID> {
    List<DirectoryEntity> findByParent(DirectoryEntity parent);
}
