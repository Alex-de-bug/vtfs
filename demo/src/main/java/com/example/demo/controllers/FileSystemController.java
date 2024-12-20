package com.example.demo.controllers;


import java.io.IOException;
import java.util.List;
import java.util.UUID;

import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.multipart.MultipartFile;

import com.example.demo.models.DirectoryEntity;
import com.example.demo.models.FileEntity;
import com.example.demo.services.FileSystemService;

@RestController
@RequestMapping("/api/filesystem")
public class FileSystemController {
    private final FileSystemService fileSystemService;

    public FileSystemController(FileSystemService fileSystemService) {
        this.fileSystemService = fileSystemService;
    }

    @PostMapping("/directory")
    public ResponseEntity<DirectoryEntity> createDirectory(@RequestParam String name, @RequestParam(required = false) UUID parentId) {
        return ResponseEntity.ok(fileSystemService.createDirectory(name, parentId));
    }

    @PostMapping("/file")
    public ResponseEntity<FileEntity> createFile(
            @RequestParam String name,
            @RequestParam MultipartFile file,
            @RequestParam(required = false) UUID parentId,
            @RequestParam(required = false) List<String> tags
    ) throws IOException {
        return ResponseEntity.ok(fileSystemService.createFile(name, file.getBytes(), parentId, tags));
    }

    @GetMapping("/directory/{dirId}/files")
    public ResponseEntity<List<FileEntity>> listFiles(@PathVariable UUID dirId) {
        return ResponseEntity.ok(fileSystemService.listFiles(dirId));
    }

    @GetMapping("/directory/{dirId}/directories")
    public ResponseEntity<List<DirectoryEntity>> listDirectories(@PathVariable UUID dirId) {
        return ResponseEntity.ok(fileSystemService.listDirectories(dirId));
    }
}
