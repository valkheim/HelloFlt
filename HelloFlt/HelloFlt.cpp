#include <fltKernel.h>

#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode drivers")

EXTERN_C_START
DRIVER_INITIALIZE DriverEntry;
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath);

NTSTATUS FilterUnloadCallback(_In_ FLT_FILTER_UNLOAD_FLAGS Flags);
NTSTATUS InstanceSetupCallback(_In_ PCFLT_RELATED_OBJECTS FltObjects, _In_ FLT_INSTANCE_SETUP_FLAGS Flags, _In_ DEVICE_TYPE VolumeDeviceType, _In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType);
NTSTATUS InstanceQueryTeardownCallback(_In_ PCFLT_RELATED_OBJECTS FltObjects, _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags);
VOID InstanceTeardownStartCallback(_In_ PCFLT_RELATED_OBJECTS FltObjects, _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags);
VOID InstanceTeardownCompleteCallback(_In_ PCFLT_RELATED_OBJECTS FltObjects, _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags);
NTSTATUS GenerateFileNameCallback(_In_ PFLT_INSTANCE Instance, _In_ PFILE_OBJECT FileObject, _In_opt_ PFLT_CALLBACK_DATA CallbackData, _In_ FLT_FILE_NAME_OPTIONS NameOptions, _Out_ PBOOLEAN CacheFileNameInformation, _Out_ PFLT_NAME_CONTROL FileName);
NTSTATUS NormalizeNameComponentCallback(_In_ PFLT_INSTANCE Instance, _In_ PCUNICODE_STRING ParentDirectory, _In_ USHORT VolumeNameLength, _In_ PCUNICODE_STRING Component, _Out_writes_bytes_(ExpandComponentNameLength) PFILE_NAMES_INFORMATION ExpandComponentName, _In_ ULONG ExpandComponentNameLength, _In_ FLT_NORMALIZE_NAME_FLAGS Flags, _Inout_ PVOID* NormalizationContext);
VOID NormalizeContextCleanupCallback(_In_opt_ PVOID* NormalizationContext);
NTSTATUS TransactionNotificationCallback(_In_ PCFLT_RELATED_OBJECTS FltObjects, _In_ PFLT_CONTEXT TransactionContext, _In_ NOTIFICATION_MASK NotificationMask);
NTSTATUS NormalizeNameComponentExCallback(_In_ PFLT_INSTANCE Instance, _In_ PFILE_OBJECT FileObject, _In_ PCUNICODE_STRING ParentDirectory, _In_ USHORT VolumeNameLength, _In_ PCUNICODE_STRING Component, _Out_writes_bytes_(ExpandComponentNameLength) PFILE_NAMES_INFORMATION ExpandComponentName, _In_ ULONG ExpandComponentNameLength, _In_ FLT_NORMALIZE_NAME_FLAGS Flags, _Inout_ PVOID* NormalizationContext);
NTSTATUS SectionNotificationCallback(_In_ PFLT_INSTANCE Instance, _In_ PFLT_CONTEXT SectionContext, _In_ PFLT_CALLBACK_DATA Data);

FLT_PREOP_CALLBACK_STATUS PreCreateOperation(_Inout_ PFLT_CALLBACK_DATA Data, _In_ PCFLT_RELATED_OBJECTS FltObjects, _Flt_CompletionContext_Outptr_ PVOID* CompletionContext);
FLT_POSTOP_CALLBACK_STATUS PostCreateOperation(_Inout_ PFLT_CALLBACK_DATA Data, _In_ PCFLT_RELATED_OBJECTS FltObjects, _In_opt_ PVOID CompletionContext, _In_ FLT_POST_OPERATION_FLAGS Flags);
EXTERN_C_END

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, FilterUnloadCallback)
#pragma alloc_text(PAGE, InstanceSetupCallback)
#pragma alloc_text(PAGE, InstanceQueryTeardownCallback)
#pragma alloc_text(PAGE, InstanceTeardownStartCallback)
#pragma alloc_text(PAGE, InstanceTeardownCompleteCallback)
#pragma alloc_text(PAGE, GenerateFileNameCallback)
#pragma alloc_text(PAGE, NormalizeNameComponentCallback)
#pragma alloc_text(PAGE, NormalizeContextCleanupCallback)
#pragma alloc_text(PAGE, TransactionNotificationCallback)
#pragma alloc_text(PAGE, NormalizeNameComponentExCallback)
#pragma alloc_text(PAGE, SectionNotificationCallback)
#endif

CONST FLT_OPERATION_REGISTRATION Callbacks[] = {            // The minifilter driver usees callbacks to indicate which operations it's interested in
    {
        IRP_MJ_CREATE,                                      // Major function code to identify the operation 
        0,                                                  // Set of flags affecting read and write operations
                                                            // FLTFL_OPERATION_REGISTRATION_SKIP_CACHED_IO: bypass callbacks if it's cached I/O (e.g. Fast I/O)
                                                            // FLTFL_OPERATION_REGISTRATION_SKIP_PAGING_IO: bypass callbacks for paging I/O (IRP-based operations)
                                                            // FLTFL_OPERATION_REGISTRATION_SKIP_NON_DASD_IO: bypass callbacks for direct access volumes (DAX/DAS)
        (PFLT_PRE_OPERATION_CALLBACK)PreCreateOperation,    // Pre operation
        (PFLT_POST_OPERATION_CALLBACK)PostCreateOperation   // Post operation
    },
    { IRP_MJ_OPERATION_END }
};

FLT_PREOP_CALLBACK_STATUS PreCreateOperation(_Inout_ PFLT_CALLBACK_DATA Data, _In_ PCFLT_RELATED_OBJECTS FltObjects, _Flt_CompletionContext_Outptr_ PVOID* CompletionContext)
{
    UNREFERENCED_PARAMETER(Data);               // Pointer to the callback data structure for the I/O operation
    UNREFERENCED_PARAMETER(FltObjects);         // Pointer to an FLT_RELATED_OBJECTS strcture that contains opaque pointers for the objects related to the current I/O request
    UNREFERENCED_PARAMETER(CompletionContext);  // Pointer to an optional context in case this callacks returns FLT_PREOP_SUCCESS_WITH_CALLBACK or FLT_PREOP_SYNCHRONIZE
    return FLT_PREOP_COMPLETE;
}

FLT_POSTOP_CALLBACK_STATUS PostCreateOperation(_Inout_ PFLT_CALLBACK_DATA Data, _In_ PCFLT_RELATED_OBJECTS FltObjects, _In_opt_ PVOID CompletionContext, _In_ FLT_POST_OPERATION_FLAGS Flags)
{
    UNREFERENCED_PARAMETER(Data);               // Pointer to the callback data structure for the I/O operation
    UNREFERENCED_PARAMETER(FltObjects);         // Pointer to an FLT_RELATED_OBJECTS strcture that contains opaque pointers for the objects related to the current I/O request
    UNREFERENCED_PARAMETER(CompletionContext);  // A pointer that was returned by the minifilter pre-operation callback.
    UNREFERENCED_PARAMETER(Flags);              // A bitmask of flags that specifies how the post-operation callback is to be performed
    return FLT_POSTOP_FINISHED_PROCESSING;
}

NTSTATUS FilterUnloadCallback(_In_ FLT_FILTER_UNLOAD_FLAGS Flags)
{
    /*
        The filter manager calls the FilterUnloadCallback routine to notify the minifilter driver that the filter manager is about to unload the minifilter driver.
    */
    UNREFERENCED_PARAMETER(Flags);              // A bitmask of flags describing the unload request
    PAGED_CODE();
    return STATUS_SUCCESS;
}

NTSTATUS InstanceSetupCallback(_In_ PCFLT_RELATED_OBJECTS FltObjects, _In_ FLT_INSTANCE_SETUP_FLAGS Flags, _In_ DEVICE_TYPE VolumeDeviceType, _In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType)
{
    /*
        The filter manager calls this routine on the first operation after a new volume is mounted.
        The filter manager calls this routine to allow the minifilter driver to respond to an automatic or manual attachment request.
        If this routine returns an error or warning NTSTATUS code, the minifilter driver instance is not attached to the given volume.
        Otherwise, the minifilter driver instance is attached to the given volume.
    */
    UNREFERENCED_PARAMETER(FltObjects);             // Pointer to an FLT_RELATED_OBJECTS structure that contains opaque pointers for the objects related to the current operation.
    UNREFERENCED_PARAMETER(Flags);                  // Bitmask of flags that indicate why the instance is being attached
    UNREFERENCED_PARAMETER(VolumeDeviceType);       // Device type of the file system volume (FILE_DEVICE_CD_ROM_FILE_SYSTEM, FILE_DEVICE_DISK_FILE_SYSTEM, FILE_DEVICE_NETWORK_FILE_SYSTEM)
    UNREFERENCED_PARAMETER(VolumeFilesystemType);   // File system type of the volume (FLT_FILESYSTEM_TYPE enum)
    PAGED_CODE();
    return STATUS_SUCCESS;
}


NTSTATUS InstanceQueryTeardownCallback(_In_ PCFLT_RELATED_OBJECTS FltObjects, _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags)
{
    /*
        The filter manager calls this routine to allow the minifilter driver to respond to a manual detach request.
        Such a request is received when a user-mode application calls FilterDetach or a kernel-mode component calls FltDetachVolume.
        This routine is not called for automatic or mandatory detach requests, for example, when the minifilter driver is unloaded or a volume is dismounted.
    */
    UNREFERENCED_PARAMETER(FltObjects);             // Pointer to an FLT_RELATED_OBJECTS structure that contains opaque pointers for the objects related to the current operation.
    UNREFERENCED_PARAMETER(Flags);                  // Bitmask of flags that describe why the given isntance query teardown callback routine was called (no flags currently defined)
    PAGED_CODE();
    return STATUS_SUCCESS;
}

VOID InstanceTeardownStartCallback(_In_ PCFLT_RELATED_OBJECTS FltObjects, _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags)
{
    /*
        The InstanceTeardownStartCallback routine is called when the filter manager starts tearing down a minifilter driver instance to allow
        the minifilter driver to complete any pended I/O operations and save state information.
        The InstanceTeardownStartCallback routine must:
            * Call FltCompletePendedPreOperation for each I/O operation that was pended in the minifilter driver's preoperation callback routine to complete the operation or return control of the operation to the filter manager.
            * Not pend any new I/O operations. If the minifilter driver uses a callback data queue, it must call FltCbdqDisable to disable it.
            * Call FltCompletePendedPostOperation for each I/O operation that was pended in the minifilter driver's postoperation callback routine to return control of the operation to the filter manager.
    */
    UNREFERENCED_PARAMETER(FltObjects);             // Pointer to an FLT_RELATED_OBJECTS structure that contains opaque pointers for the objects related to the current operation.
    UNREFERENCED_PARAMETER(Flags);                  // Flag that indicates why the minifilter driver instance is being torn down.
    PAGED_CODE();
}

VOID InstanceTeardownCompleteCallback(_In_ PCFLT_RELATED_OBJECTS FltObjects, _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags)
{
    /*
        The InstanceTeardownCompleteCallback routine is called when the teardown process is complete to allow
        the minifilter driver to close open files and perform any other necessary cleanup processing
        The InstanceTeardownCompleteCallback routine must close any files that were opened by the minifilter driver.
    */
    UNREFERENCED_PARAMETER(FltObjects);             // Pointer to an FLT_RELATED_OBJECTS structure that contains opaque pointers for the objects related to the current operation.
    UNREFERENCED_PARAMETER(Flags);                  // Flag that indicates why the minifilter driver instance is being torn down.
    PAGED_CODE();
}


NTSTATUS GenerateFileNameCallback(_In_ PFLT_INSTANCE Instance, _In_ PFILE_OBJECT FileObject, _In_opt_ PFLT_CALLBACK_DATA CallbackData, _In_ FLT_FILE_NAME_OPTIONS NameOptions, _Out_ PBOOLEAN CacheFileNameInformation, _Out_ PFLT_NAME_CONTROL FileName)
{
    /*
        The filter manager calls this callback routine to allow the minifilter driver to intercept file name requests by other minifilter drivers
        above it in the minifilter driver instance stack. Using this callback routine and the PFLT_NORMALIZE_NAME_COMPONENT callback routine,
        the minifilter driver can provide its own file name information.
    */
    UNREFERENCED_PARAMETER(Instance);                   // Opaque instance pointer for the minifilter driver instance that this callback routine is registered for.
    UNREFERENCED_PARAMETER(FileObject);                 // A pointer to a file object for the file whose name is being requested.
    UNREFERENCED_PARAMETER(CallbackData);               // A pointer to the callback data structure for the operation during which this name is being requested.
    UNREFERENCED_PARAMETER(NameOptions);                // FLT_FILE_NAME_OPTIONS value that specifies the name format, query method, and flags for this file name information query.
    UNREFERENCED_PARAMETER(CacheFileNameInformation);   // A pointer to a Boolean value specifying whether this name can be cached.
    UNREFERENCED_PARAMETER(FileName);                   // A pointer to a filter manager-allocated FLT_NAME_CONTROL structure to receive the file name on output.
    PAGED_CODE();
    return STATUS_SUCCESS;
}

NTSTATUS NormalizeNameComponentCallback(_In_ PFLT_INSTANCE Instance, _In_ PCUNICODE_STRING ParentDirectory, _In_ USHORT VolumeNameLength, _In_ PCUNICODE_STRING Component, _Out_writes_bytes_(ExpandComponentNameLength) PFILE_NAMES_INFORMATION ExpandComponentName, _In_ ULONG ExpandComponentNameLength, _In_ FLT_NORMALIZE_NAME_FLAGS Flags, _Inout_ PVOID* NormalizationContext)
{
    /*
        The filter manager calls this callback routine to query the minifilter driver for the normalized names for components in the file name path whose names the minifilter driver has modified.
        If the file name path contains more than one such component, the filter manager can call this callback routine multiple times in the process of normalizing all the components in the path.
        The minifilter driver can use the NormalizationContext parameter to pass context information to subsequent calls to this callback routine.
    */
    UNREFERENCED_PARAMETER(Instance);                   // Opaque instance pointer for the minifilter driver instance that this callback routine is registered for.
    UNREFERENCED_PARAMETER(ParentDirectory);            // Pointer to a UNICODE_STRING structure that contains the name of the parent directory for this name component.
    UNREFERENCED_PARAMETER(VolumeNameLength);           // Length, in bytes, of the parent directory name stored in the structure that the ParentDirectory parameter points to.
    UNREFERENCED_PARAMETER(Component);                  // Pointer to a UNICODE_STRING structure that contains the name component to be expanded.
    UNREFERENCED_PARAMETER(ExpandComponentName);        // Pointer to a FILE_NAMES_INFORMATION structure that receives the expanded (normalized) file name information for the name component.
    UNREFERENCED_PARAMETER(ExpandComponentNameLength);  // Length, in bytes, of the buffer that the ExpandComponentName parameter points to.
    UNREFERENCED_PARAMETER(Flags);                      // Name normalization flags (FLTFL_NORMALIZE_NAME_CASE_SENSITIVE, FLTFL_NORMALIZE_NAME_DESTINATION_FILE_NAME).
    UNREFERENCED_PARAMETER(NormalizationContext);       // Pointer to minifilter driver-provided context information to be passed in any subsequent calls to this callback routine that are made to normalize the remaining components in the same file name path.
    PAGED_CODE();
    return STATUS_SUCCESS;
}

VOID NormalizeContextCleanupCallback(_In_opt_ PVOID* NormalizationContext)
{
    /*
        The filter manager calls this callback routine to allow the minifilter driver to perform any needed
        cleanup for the context information passed in the NormalizationContext parameter of the PFLT_NORMALIZE_NAME_COMPONENT callback routine.
        This context is set by the minifilter driver when its PFLT_NORMALIZE_NAME_COMPONENT callback routine is called.
    */
    UNREFERENCED_PARAMETER(NormalizationContext);       // Pointer to minifilter driver-provided context information to be passed in any calls to the PFLT_NORMALIZE_NAME_COMPONENT callback routine (NormalizeNameComponentCallback) that are made to normalize multiple components in the same file name path.
    PAGED_CODE();
}

NTSTATUS TransactionNotificationCallback(_In_ PCFLT_RELATED_OBJECTS FltObjects, _In_ PFLT_CONTEXT TransactionContext, _In_ NOTIFICATION_MASK NotificationMask)
{
    /*
        The filter manager calls this routine to notify the minifilter driver about the status of a transaction that the minifilter driver is enlisted in.
    */
    UNREFERENCED_PARAMETER(FltObjects);         // Pointer to an FLT_RELATED_OBJECTS structure that contains opaque pointers for the objects related to the current operation.
    UNREFERENCED_PARAMETER(TransactionContext); // Pointer to the minifilter driver's transaction context.
    UNREFERENCED_PARAMETER(NotificationMask);   // Specifies the type of notifications that the filter manager is sending to the minifilter driver, as one of the following values.
    PAGED_CODE();
    return STATUS_SUCCESS;
}

NTSTATUS NormalizeNameComponentExCallback(_In_ PFLT_INSTANCE Instance, _In_ PFILE_OBJECT FileObject, _In_ PCUNICODE_STRING ParentDirectory, _In_ USHORT VolumeNameLength, _In_ PCUNICODE_STRING Component, _Out_writes_bytes_(ExpandComponentNameLength) PFILE_NAMES_INFORMATION ExpandComponentName, _In_ ULONG ExpandComponentNameLength, _In_ FLT_NORMALIZE_NAME_FLAGS Flags, _Inout_ PVOID* NormalizationContext)
{
    /*
        The filter manager calls this callback routine to query the minifilter driver for the normalized names for components in the file name path whose names the minifilter driver has modified.
        If the file name path contains more than one such component, the filter manager can call this callback routine multiple times in the process of normalizing all the components in the path.
        The minifilter driver can use the NormalizationContext parameter to pass context information to subsequent calls to this callback routine.

        The principal difference between the NormalizeNameComponentExCallback callback routine and the NormalizeNameComponentCallback callback routine (of type PFLT_NORMALIZE_NAME_COMPONENT) is that
        the NormalizeNameComponentExCallback callback routine supports the additional FileObject parameter.
    */
    UNREFERENCED_PARAMETER(Instance);                   // Opaque instance pointer for the minifilter driver instance that this callback routine is registered for.
    UNREFERENCED_PARAMETER(FileObject);                 // Pointer to the file object for the file whose name is being requested or the file that is the target of the IRP_MJ_SET_INFORMATION operation if the FLTFL_NORMALIZE_NAME_DESTINATION_FILE_NAME flag is set.
    UNREFERENCED_PARAMETER(ParentDirectory);            // Pointer to a UNICODE_STRING structure that contains the name of the parent directory for this name component.
    UNREFERENCED_PARAMETER(VolumeNameLength);           // Length, in bytes, of the parent directory name that is stored in the structure that the ParentDirectory parameter points to.
    UNREFERENCED_PARAMETER(Component);                  // Pointer to a UNICODE_STRING structure that contains the name component to be expanded.
    UNREFERENCED_PARAMETER(ExpandComponentName);        // Pointer to a FILE_NAMES_INFORMATION structure that receives the expanded (normalized) file name information for the name component.
    UNREFERENCED_PARAMETER(ExpandComponentNameLength);  // Length, in bytes, of the buffer that the ExpandComponentName parameter points to.
    UNREFERENCED_PARAMETER(Flags);                      // Name normalization flags.
    UNREFERENCED_PARAMETER(NormalizationContext);       // Pointer to minifilter driver-provided context information to be passed in any subsequent calls to this callback routine that are made to normalize the remaining components in the same file name path.
    PAGED_CODE();
    return STATUS_SUCCESS;
}

NTSTATUS SectionNotificationCallback(_In_ PFLT_INSTANCE Instance, _In_ PFLT_CONTEXT SectionContext, _In_ PFLT_CALLBACK_DATA Data)
{
    /*
        A minifilter can use this to register for section conflict notifications.
        Certain situations can occur where holding a section open is incompatible with current file I/O. In particular, file I/O that triggers a cache purge can cause cache incoherency if the cache purge is prevented because of an open section.
        A minifilter can provide this optional callback routine for notifications of these events.
        When a notification is received, the section can be closed to allow the conflicting I/O operation to continue.
    */
    UNREFERENCED_PARAMETER(Instance);                   // An opaque instance pointer to the minifilter driver instance that is initiating the I/O operation.
    UNREFERENCED_PARAMETER(SectionContext);             // A pointer to the section context that incurred a data scan section conflict.
    UNREFERENCED_PARAMETER(Data);                       // A pointer to a caller-allocated structure that contains the callback data.
    PAGED_CODE();
    return STATUS_SUCCESS;
}

const FLT_REGISTRATION FilterRegistration = {                                   // A driver uses FLT_REGISTRATION to register itself with the filter manager
    sizeof(FLT_REGISTRATION),                                                   // Size
    FLT_REGISTRATION_VERSION,                                                   // Version
    (FLT_REGISTRATION_FLAGS)0,                                                  // Flags
    (PFLT_CONTEXT_REGISTRATION)nullptr,                                         // Pointer to contexts where each represents a context that the driver may use in its work. Context refers to some driver-defined data that can be attached to file system entries, such as files and volumes
    (PFLT_OPERATION_REGISTRATION)Callbacks,                                     // Operation callbacks, each specifying and operation of interest and associated pre/post callbacks
    (PFLT_FILTER_UNLOAD_CALLBACK)FilterUnloadCallback,                          // Function to be called when the driver is about to be unloaded
    (PFLT_INSTANCE_SETUP_CALLBACK)InstanceSetupCallback,                        // Allows the driver to be notified when an instance is about to be attached to a new volume
    (PFLT_INSTANCE_QUERY_TEARDOWN_CALLBACK)InstanceQueryTeardownCallback,       // InstanceQueryTeardown
    (PFLT_INSTANCE_TEARDOWN_CALLBACK)InstanceTeardownStartCallback,             // (opt) InstanceTeardownStart
    (PFLT_INSTANCE_TEARDOWN_CALLBACK)InstanceTeardownCompleteCallback,          // (opt) InstanceTeardownComplete
    (PFLT_GENERATE_FILE_NAME)GenerateFileNameCallback,                          // (opt) GenerateFileNameCallback
    (PFLT_NORMALIZE_NAME_COMPONENT)NormalizeNameComponentCallback,              // (opt) NormalizeNameComponentCallback
    (PFLT_NORMALIZE_CONTEXT_CLEANUP)NormalizeContextCleanupCallback,            // (opt) NormalizeContextCleanupCallback
    (PFLT_TRANSACTION_NOTIFICATION_CALLBACK)TransactionNotificationCallback,    // (opt) TransactionNotificationCallback
    (PFLT_NORMALIZE_NAME_COMPONENT_EX)NormalizeNameComponentExCallback,         // (opt) NormalizeNameComponentExCallback
#if FLT_MGR_WIN8
    (PFLT_SECTION_CONFLICT_NOTIFICATION_CALLBACK)SectionNotificationCallback,   // (opt) SectionNotificationCallback
#endif
};

PFLT_FILTER FilterHandle;


NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);
    auto status = FltRegisterFilter(    // Registers a minifilter driver
        DriverObject,                   // Pointer to the driver object for the minifilter driver
        &FilterRegistration,            // Pointer to a minifilter driver registration structure
        &FilterHandle                   // Pointer to a variable that receives an opaque filter pointer for the caller
    );

    FLT_ASSERT(NT_SUCCESS(status));
    if (!NT_SUCCESS(status))
        return status;

    // FltCreateCommunicationPort
    status = FltStartFiltering(FilterHandle);
    if (!NT_SUCCESS(status))
        FltUnregisterFilter(FilterHandle);

    return status;
}