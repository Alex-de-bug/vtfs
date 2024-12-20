package com.example.demo.services;

import java.util.List;
import java.util.UUID;

import org.springframework.stereotype.Service;

import com.example.demo.models.DirectoryEntity;
import com.example.demo.models.FileEntity;
import com.example.demo.repositories.DirectoryRepository;
import com.example.demo.repositories.FileRepository;

@Service
public class FileSystemService {
    private final FileRepository fileRepository;
    private final DirectoryRepository directoryRepository;

    public FileSystemService(FileRepository fileRepository, DirectoryRepository directoryRepository) {
        this.fileRepository = fileRepository;
        this.directoryRepository = directoryRepository;
    }

    public DirectoryEntity createDirectory(String name, UUID parentId) {
        DirectoryEntity parent = parentId != null ? directoryRepository.findById(parentId).orElseThrow() : null;
        DirectoryEntity directory = new DirectoryEntity();
        directory.setName(name);
        directory.setParent(parent);
        return directoryRepository.save(directory);
    }

    public FileEntity createFile(String name, byte[] content, UUID parentId, List<String> tags) {
        DirectoryEntity parent = parentId != null ? directoryRepository.findById(parentId).orElseThrow() : null;
        FileEntity file = new FileEntity();
        file.setName(name);
        file.setContent(content);
        file.setParent(parent);
        return fileRepository.save(file);
    }

    public void deleteFile(UUID fileId) {
        fileRepository.deleteById(fileId);
    }

    public void deleteDirectory(UUID dirId) {
        directoryRepository.deleteById(dirId);
    }

    public List<FileEntity> listFiles(UUID dirId) {
        DirectoryEntity parent = dirId != null ? directoryRepository.findById(dirId).orElseThrow() : null;
        return fileRepository.findByParent(parent);
    }

    public List<DirectoryEntity> listDirectories(UUID dirId) {
        DirectoryEntity parent = dirId != null ? directoryRepository.findById(dirId).orElseThrow() : null;
        return directoryRepository.findByParent(parent);
    }
}

