package com.decafmango.nfs_server.model;

import lombok.AllArgsConstructor;
import lombok.Getter;

@AllArgsConstructor
@Getter
public enum DentryMode {
    
    DT_DIR((byte) 5),
    DT_REG((byte) 3);

    private final byte mode;

}
