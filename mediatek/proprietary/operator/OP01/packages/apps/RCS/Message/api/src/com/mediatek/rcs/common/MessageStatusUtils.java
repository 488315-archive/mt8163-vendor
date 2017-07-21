package com.mediatek.rcs.common;

/**
 * MessageStatusUtils contains a internal class IFileTransfer and a enumerate status.
 * {@link #IFileTransfer} defines the RCS status, it include a file transfer in pending,
 * sending, waiting status and so on.
 * {@link #Status} defines the status of a sent message, it is mainly for updating
 * database.
 */
public class MessageStatusUtils {

    public static final class IFileTransfer {
        public static enum Status {
            /**
             * The file transfer invitation has to wait until the active queue
             * is available
             */
            PENDING,
            /**
             * The file transfer invitation is waiting for the acceptation
             */
            WAITING,
            /**
             * The file transfer is on-going
             */
            TRANSFERING,
            /**
             * The file transfer is canceled by current user
             */
            CANCEL,
            /**
             * The file transfer is canceled by the remote contact
             */
            CANCELED,
            /**
             * The file transfer has failed
             */
            FAILED,
            /**
             * The file transfer has been rejected
             */
            REJECTED,
            /**
             * The file transfer has been done with success
             */
            FINISHED,
            /**
             * The file transfer has been timeout with no response from receiver
             */
            TIMEOUT,
        }
    }

    /**
     * This enumerate defines the status of a sent message
     */

}