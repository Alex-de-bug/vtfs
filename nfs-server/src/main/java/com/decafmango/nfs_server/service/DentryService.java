package com.decafmango.nfs_server.service;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.List;
import java.util.Optional;
import java.util.concurrent.atomic.AtomicInteger;

import org.springframework.stereotype.Service;

import com.decafmango.nfs_server.dao.DentryRepository;
import com.decafmango.nfs_server.model.Dentry;
import com.decafmango.nfs_server.model.DentryMode;
import com.decafmango.nfs_server.model.ServerResponse;

import jakarta.annotation.PostConstruct;
import lombok.RequiredArgsConstructor;

@Service
@RequiredArgsConstructor
public class DentryService {

    private final DentryRepository repository;

    private AtomicInteger maxInode;

    @PostConstruct
    private void init() {
        Integer inode = repository.findMaxInode();
        if (inode == null)
            maxInode = new AtomicInteger(100);
        else
            maxInode = new AtomicInteger(inode);
    }

    public byte[] maxIno() {
        ByteBuffer buffer = ByteBuffer.allocate(8 + 4);
        buffer.order(ByteOrder.LITTLE_ENDIAN);

        buffer.putLong(ServerResponse.OK.getCode());
        buffer.putInt(maxInode.get());

        return buffer.array();
    }

    public byte[] lookup(int parentInode, String name) {
        ByteBuffer buffer = ByteBuffer.allocate(8 + 4 + 1);
        buffer.order(ByteOrder.LITTLE_ENDIAN);

        Optional<Dentry> dentryOptional = repository.findByParentInodeAndName(parentInode, name);
        if (dentryOptional.isEmpty()) {
            buffer.putLong(ServerResponse.NOT_FOUND.getCode());
            return buffer.array();
        }

        Dentry dentry = dentryOptional.get();

        buffer.putLong(ServerResponse.OK.getCode());
        buffer.putInt(dentry.getInode());
        buffer.put(dentry.getMode());

        return buffer.array();
    }

    public byte[] iterate(int parentInode) {
        ByteBuffer buffer = ByteBuffer.allocate(8 + 4 + 8 * (256 + 4 + 1));
        buffer.order(ByteOrder.LITTLE_ENDIAN);

        if (!repository.existsByInode(parentInode)) {
            buffer.putLong(ServerResponse.NOT_FOUND.getCode());
            return buffer.array();
        }        

        List<Dentry> dentries = repository.findAllByParentInode(parentInode);

        buffer.putLong(ServerResponse.OK.getCode());
        buffer.putInt(dentries.size());

        for (Dentry dentry : dentries) {
            byte[] name = Arrays.copyOf(dentry.getName().getBytes(StandardCharsets.US_ASCII), 256);
            name[255] = 0;
            buffer.put(name);
            buffer.putInt(dentry.getInode());
            buffer.put(dentry.getMode());
        }

        return buffer.array();
    }

    public byte[] create(int parentInode, String name, byte mode) {
        ByteBuffer buffer = ByteBuffer.allocate(8 + 4);
        buffer.order(ByteOrder.LITTLE_ENDIAN);

        if (mode != DentryMode.DT_REG.getMode() && mode != DentryMode.DT_DIR.getMode()) {
            System.out.println("NOTHING1");
            buffer.putLong(ServerResponse.UNKNOWN.getCode());
            return buffer.array();
        }

        if (!repository.existsByInode(parentInode)) {
            System.out.println(parentInode);
            buffer.putLong(ServerResponse.NOT_FOUND.getCode());
            return buffer.array();
        }

        if (repository.findByParentInodeAndName(parentInode, name).isPresent()) {
            System.out.println("NOTHING2");
            buffer.putLong(ServerResponse.CONFLICT.getCode());
            return buffer.array();
        }

        int newInode = maxInode.incrementAndGet();

        Dentry newNode = new Dentry();
        newNode.setInode(newInode);
        newNode.setName(name);
        newNode.setParentInode(parentInode);
        newNode.setMode(mode);
        newNode.setData("");

        repository.save(newNode);

        buffer.putLong(ServerResponse.OK.getCode());
        buffer.putInt(newInode);
        return buffer.array();
    }

    public byte[] remove(int parentInode, String name) {
        ByteBuffer buffer = ByteBuffer.allocate(8);
        buffer.order(ByteOrder.LITTLE_ENDIAN);

        if (!repository.existsByInode(parentInode)) {
            buffer.putLong(ServerResponse.NOT_FOUND.getCode());
            return buffer.array();
        }

        Optional<Dentry> dentryOptional = repository.findByParentInodeAndName(parentInode, name);

        if (dentryOptional.isEmpty()) {
            buffer.putLong(ServerResponse.NOT_FOUND.getCode());
            return buffer.array();
        }

        Dentry dentry = dentryOptional.get();

        if (dentry.getMode() == DentryMode.DT_DIR.getMode() && repository.existsByParentInode(parentInode)) {
            buffer.putLong(ServerResponse.CONFLICT.getCode());
            return buffer.array();
        }

        repository.delete(dentry);

        buffer.putLong(ServerResponse.OK.getCode());
        return buffer.array();
    }

    public byte[] read(int inode) {
        ByteBuffer buffer = ByteBuffer.allocate(8 + 4 + 2048);
        buffer.order(ByteOrder.LITTLE_ENDIAN);

        if (!repository.existsByInode(inode)) {
            buffer.putLong(ServerResponse.NOT_FOUND.getCode());
            return buffer.array();
        }

        Optional<Dentry> dentryOptional = repository.findByInode(inode);

        if (dentryOptional.isEmpty()) {
            buffer.putLong(ServerResponse.NOT_FOUND.getCode());
            return buffer.array();
        }

        Dentry dentry = dentryOptional.get();

        if (dentry.getMode() == DentryMode.DT_DIR.getMode()) {
            buffer.putLong(ServerResponse.CONFLICT.getCode());
            return buffer.array();
        }

        buffer.putLong(ServerResponse.OK.getCode());

        byte[] data = dentry.getData().getBytes(StandardCharsets.US_ASCII);
        buffer.putInt(data.length);

        byte[] dataBuffer = Arrays.copyOf(data, 2048);
        dataBuffer[2047] = 0;

        buffer.put(dataBuffer);
        return buffer.array();        
    }

    public byte[] write(int inode, String data) {
        ByteBuffer buffer = ByteBuffer.allocate(8);
        buffer.order(ByteOrder.LITTLE_ENDIAN);

        Optional<Dentry> dentryOptional = repository.findByInode(inode);

        if (dentryOptional.isEmpty()) {
            buffer.putLong(ServerResponse.NOT_FOUND.getCode());
            return buffer.array();
        }

        Dentry dentry = dentryOptional.get();
        dentry.setData(data);
        repository.save(dentry);

        buffer.putLong(ServerResponse.OK.getCode());
        return buffer.array();
    }
    
}
