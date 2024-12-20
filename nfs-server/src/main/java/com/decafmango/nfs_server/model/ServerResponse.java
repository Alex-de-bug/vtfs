package com.decafmango.nfs_server.model;

import lombok.AllArgsConstructor;
import lombok.Getter;

@AllArgsConstructor
@Getter
public enum ServerResponse {
    
    OK(0),
    NOT_FOUND(1),
    CONFLICT(2),
    UNKNOWN(3);


    private final long code;

}
