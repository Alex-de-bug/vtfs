package com.decafmango.nfs_server.controller;

import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import com.decafmango.nfs_server.service.DentryService;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;

@RestController
@RequestMapping("/api")
@RequiredArgsConstructor
@Slf4j
public class DentryController {

    private final DentryService service;

    @GetMapping("/max_ino")
    public ResponseEntity<byte[]> maxIno() {
        log.info("GET /api/max_ino");

        return ResponseEntity.ok().body(service.maxIno());
    }

    @GetMapping("/lookup")
    public ResponseEntity<byte[]> lookup(
            @RequestParam("parentInode") String parentInodeStr,
            @RequestParam("name") String name) {
        log.info("GET /api/iterate parent=\"{}\", name=\"{}\"", parentInodeStr, name);

        int parentInode;
        try {
            parentInode = Integer.parseInt(parentInodeStr);
        } catch (NumberFormatException e) {
            return ResponseEntity.status(HttpStatus.BAD_REQUEST).build();
        }

        return ResponseEntity.ok().body(service.lookup(parentInode, name));
    }

    @GetMapping("/iterate")
    public ResponseEntity<byte[]> iterate(
            @RequestParam("parentInode") String parentInodeStr) {
        log.info("GET /api/iterate inodeInode=\"{}\"", parentInodeStr);
        
        int parentInode;
        try {
            parentInode = Integer.parseInt(parentInodeStr);
        } catch (NumberFormatException e) {
            return ResponseEntity.badRequest().build();
        }
        return ResponseEntity.ok(service.iterate(parentInode));

    }

    @GetMapping("/create")
    public ResponseEntity<byte[]> create(
        @RequestParam("parentInode") String parentInodeStr,
        @RequestParam("name") String name,
        @RequestParam("mode") String modeStr
    ) {
        log.info("GET /api/create parentInode=\"{}\", name=\"{}\", type=\"{}\"", parentInodeStr, name, modeStr);

        int parentInode;
        byte mode;
        try {
            parentInode = Integer.parseInt(parentInodeStr);
            mode = Byte.parseByte(modeStr);
        } catch (NumberFormatException e) {
            return ResponseEntity.status(HttpStatus.BAD_REQUEST).build();
        }
        return ResponseEntity.ok().body(service.create(parentInode, name, mode));
    }

    @GetMapping("/remove")
    public ResponseEntity<byte[]> remove(
        @RequestParam("parentInode") String parentInodeStr,
        @RequestParam("name") String name
    ) {
        log.info("GET /api/remove parent=\"{}\", name=\"{}\"", parentInodeStr, name);

        int parentInode;
        try {
            parentInode = Integer.parseInt(parentInodeStr);
        } catch (NumberFormatException e) {
            return ResponseEntity.status(HttpStatus.BAD_REQUEST).build();
        }

        return ResponseEntity.ok().body(service.remove(parentInode, name));
    }

    @GetMapping("/read")
    public ResponseEntity<byte[]> read(
        @RequestParam(name = "inode") String inodeStr
    ) {
        log.info("GET /api/read inode=\"{}\"", inodeStr);

        int inode;
        try {
            inode = Integer.parseInt(inodeStr);
        } catch (NumberFormatException e) {
            return ResponseEntity.status(HttpStatus.BAD_REQUEST).build();
        }

        return ResponseEntity.ok().body(service.read(inode));
    }

    @GetMapping("/write")
    public ResponseEntity<byte[]> write(
        @RequestParam(name = "inode") String inodeStr, 
        @RequestParam(name = "data") String data
    ) {
        log.info("GET /api/write inode=\"{}\" data=\"{}\"", inodeStr, data);

        int inode;
        try {
            inode = Integer.parseInt(inodeStr);
        } catch (NumberFormatException e) {
            return ResponseEntity.status(HttpStatus.BAD_REQUEST).build();
        }

        return ResponseEntity.ok().body(service.write(inode, data));
    }

}
