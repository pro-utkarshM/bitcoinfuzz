#![no_std]
#![no_main]

extern crate alloc;
use alloc::vec::Vec;
use bitcoin::{consensus, Block};
use core::slice;

#[no_mangle]
pub extern "C" fn rust_block_deserialize(
    data: *const u8,
    len: usize,
    out_version: *mut i32,
    out_tx_count: *mut usize,
) -> bool {
    // Safety: FFI boundary
    let bytes = unsafe { slice::from_raw_parts(data, len) };
    match consensus::deserialize::<Block>(bytes) {
        Ok(block) => {
            unsafe {
                *out_version = block.header.version;
                *out_tx_count = block.txdata.len();
            }
            true
        }
        Err(_) => false,
    }
}