<?hh
/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */

namespace hack\fixtures;

module hack.module.internal;
/**
 * Original thrift service:-
 * TestServiceWithServiceAnnotation
 */
interface TestServiceWithServiceAnnotationAsyncIf extends \IThriftAsyncIf {
  /**
   * Original thrift definition:-
   * i32
   *   testMethodWithServiceAnnotation();
   */
  public function testMethodWithServiceAnnotation(): Awaitable<int>;

  /**
   * Original thrift definition:-
   * void
   *   testMethodWithServiceAnnotation2();
   */
  public function testMethodWithServiceAnnotation2(): Awaitable<void>;
}

/**
 * Original thrift service:-
 * TestServiceWithServiceAnnotation
 */
interface TestServiceWithServiceAnnotationIf extends \IThriftSyncIf {
  /**
   * Original thrift definition:-
   * i32
   *   testMethodWithServiceAnnotation();
   */
  public function testMethodWithServiceAnnotation(): int;

  /**
   * Original thrift definition:-
   * void
   *   testMethodWithServiceAnnotation2();
   */
  public function testMethodWithServiceAnnotation2(): void;
}

/**
 * Original thrift service:-
 * TestServiceWithServiceAnnotation
 */
interface TestServiceWithServiceAnnotationAsyncClientIf extends TestServiceWithServiceAnnotationAsyncIf {
}

/**
 * Original thrift service:-
 * TestServiceWithServiceAnnotation
 */
interface TestServiceWithServiceAnnotationClientIf extends \IThriftSyncIf {
  /**
   * Original thrift definition:-
   * i32
   *   testMethodWithServiceAnnotation();
   */
  public function testMethodWithServiceAnnotation(): Awaitable<int>;

  /**
   * Original thrift definition:-
   * void
   *   testMethodWithServiceAnnotation2();
   */
  public function testMethodWithServiceAnnotation2(): Awaitable<void>;
}

/**
 * Original thrift service:-
 * TestServiceWithServiceAnnotation
 */
trait TestServiceWithServiceAnnotationClientBase {
  require extends \ThriftClientBase;

}

class TestServiceWithServiceAnnotationAsyncClient extends \ThriftClientBase implements TestServiceWithServiceAnnotationAsyncClientIf {
  use TestServiceWithServiceAnnotationClientBase;

  /**
   * Original thrift definition:-
   * i32
   *   testMethodWithServiceAnnotation();
   */
  public async function testMethodWithServiceAnnotation(): Awaitable<int> {
    $hh_frame_metadata = $this->getHHFrameMetadata();
    if ($hh_frame_metadata !== null) {
      \HH\set_frame_metadata($hh_frame_metadata);
    }
    $rpc_options = $this->getAndResetOptions() ?? \ThriftClientBase::defaultOptions();
    $args = \hack\fixtures\TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation_args::withDefaultValues();
    await $this->asyncHandler_->genBefore("TestServiceWithServiceAnnotation", "testMethodWithServiceAnnotation", $args);
    $currentseqid = $this->sendImplHelper($args, "testMethodWithServiceAnnotation", false);
    return await $this->genAwaitResponse(\hack\fixtures\TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation_result::class, "testMethodWithServiceAnnotation", false, $currentseqid, $rpc_options);
  }

  /**
   * Original thrift definition:-
   * void
   *   testMethodWithServiceAnnotation2();
   */
  public async function testMethodWithServiceAnnotation2(): Awaitable<void> {
    $hh_frame_metadata = $this->getHHFrameMetadata();
    if ($hh_frame_metadata !== null) {
      \HH\set_frame_metadata($hh_frame_metadata);
    }
    $rpc_options = $this->getAndResetOptions() ?? \ThriftClientBase::defaultOptions();
    $args = \hack\fixtures\TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation2_args::withDefaultValues();
    await $this->asyncHandler_->genBefore("TestServiceWithServiceAnnotation", "testMethodWithServiceAnnotation2", $args);
    $currentseqid = $this->sendImplHelper($args, "testMethodWithServiceAnnotation2", false);
    await $this->genAwaitResponse(\hack\fixtures\TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation2_result::class, "testMethodWithServiceAnnotation2", true, $currentseqid, $rpc_options);
  }

}

class TestServiceWithServiceAnnotationClient extends \ThriftClientBase implements TestServiceWithServiceAnnotationClientIf {
  use TestServiceWithServiceAnnotationClientBase;

  /**
   * Original thrift definition:-
   * i32
   *   testMethodWithServiceAnnotation();
   */
  public async function testMethodWithServiceAnnotation(): Awaitable<int> {
    $hh_frame_metadata = $this->getHHFrameMetadata();
    if ($hh_frame_metadata !== null) {
      \HH\set_frame_metadata($hh_frame_metadata);
    }
    $rpc_options = $this->getAndResetOptions() ?? \ThriftClientBase::defaultOptions();
    $args = \hack\fixtures\TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation_args::withDefaultValues();
    await $this->asyncHandler_->genBefore("TestServiceWithServiceAnnotation", "testMethodWithServiceAnnotation", $args);
    $currentseqid = $this->sendImplHelper($args, "testMethodWithServiceAnnotation", false);
    return await $this->genAwaitResponse(\hack\fixtures\TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation_result::class, "testMethodWithServiceAnnotation", false, $currentseqid, $rpc_options);
  }

  /**
   * Original thrift definition:-
   * void
   *   testMethodWithServiceAnnotation2();
   */
  public async function testMethodWithServiceAnnotation2(): Awaitable<void> {
    $hh_frame_metadata = $this->getHHFrameMetadata();
    if ($hh_frame_metadata !== null) {
      \HH\set_frame_metadata($hh_frame_metadata);
    }
    $rpc_options = $this->getAndResetOptions() ?? \ThriftClientBase::defaultOptions();
    $args = \hack\fixtures\TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation2_args::withDefaultValues();
    await $this->asyncHandler_->genBefore("TestServiceWithServiceAnnotation", "testMethodWithServiceAnnotation2", $args);
    $currentseqid = $this->sendImplHelper($args, "testMethodWithServiceAnnotation2", false);
    await $this->genAwaitResponse(\hack\fixtures\TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation2_result::class, "testMethodWithServiceAnnotation2", true, $currentseqid, $rpc_options);
  }

  /* send and recv functions */
  public function send_testMethodWithServiceAnnotation(): int {
    $args = \hack\fixtures\TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation_args::withDefaultValues();
    return $this->sendImplHelper($args, "testMethodWithServiceAnnotation", false);
  }
  public function recv_testMethodWithServiceAnnotation(?int $expectedsequenceid = null): int {
    return $this->recvImplHelper(\hack\fixtures\TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation_result::class, "testMethodWithServiceAnnotation", false, $expectedsequenceid);
  }
  public function send_testMethodWithServiceAnnotation2(): int {
    $args = \hack\fixtures\TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation2_args::withDefaultValues();
    return $this->sendImplHelper($args, "testMethodWithServiceAnnotation2", false);
  }
  public function recv_testMethodWithServiceAnnotation2(?int $expectedsequenceid = null): void {
    $this->recvImplHelper(\hack\fixtures\TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation2_result::class, "testMethodWithServiceAnnotation2", true, $expectedsequenceid);
  }
}

// HELPER FUNCTIONS AND STRUCTURES

class TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation_args implements \IThriftSyncStruct, \IThriftStructMetadata {
  use \ThriftSerializationTrait;

  const \ThriftStructTypes::TSpec SPEC = dict[
  ];
  const dict<string, int> FIELDMAP = dict[
  ];

  const type TConstructorShape = shape(
  );

  const int STRUCTURAL_ID = 957977401221134810;

  public function __construct()[] {
  }

  public static function withDefaultValues()[]: this {
    return new static();
  }

  public static function fromShape(self::TConstructorShape $shape)[]: this {
    return new static(
    );
  }

  public function getName()[]: string {
    return 'TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation_args';
  }

  public function clearTerseFields()[write_props]: void {
  }

  public static function getStructMetadata()[]: \tmeta_ThriftStruct {
    return \tmeta_ThriftStruct::fromShape(
      shape(
        "name" => "module.testMethodWithServiceAnnotation_args",
        "is_union" => false,
      )
    );
  }

  public static function getAllStructuredAnnotations()[write_props]: \TStructAnnotations {
    return shape(
      'struct' => dict[],
      'fields' => dict[
      ],
    );
  }

  public function getInstanceKey()[write_props]: string {
    return \TCompactSerializer::serialize($this);
  }

}

class TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation_result extends \ThriftSyncStructWithResult implements \IThriftStructMetadata {
  use \ThriftSerializationTrait;

  const type TResult = int;

  const \ThriftStructTypes::TSpec SPEC = dict[
    0 => shape(
      'var' => 'success',
      'type' => \TType::I32,
    ),
  ];
  const dict<string, int> FIELDMAP = dict[
    'success' => 0,
  ];

  const type TConstructorShape = shape(
    ?'success' => ?this::TResult,
  );

  const int STRUCTURAL_ID = 3865318819874171525;
  public ?this::TResult $success;

  public function __construct(?this::TResult $success = null)[] {
    $this->success = $success;
  }

  public static function withDefaultValues()[]: this {
    return new static();
  }

  public static function fromShape(self::TConstructorShape $shape)[]: this {
    return new static(
      Shapes::idx($shape, 'success'),
    );
  }

  public function getName()[]: string {
    return 'TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation_result';
  }

  public function clearTerseFields()[write_props]: void {
  }

  public static function getStructMetadata()[]: \tmeta_ThriftStruct {
    return \tmeta_ThriftStruct::fromShape(
      shape(
        "name" => "module.TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation_result",
        "fields" => vec[
          \tmeta_ThriftField::fromShape(
            shape(
              "id" => 0,
              "type" => \tmeta_ThriftType::fromShape(
                shape(
                  "t_primitive" => \tmeta_ThriftPrimitiveType::THRIFT_I32_TYPE,
                )
              ),
              "name" => "success",
            )
          ),
        ],
        "is_union" => false,
      )
    );
  }

  public static function getAllStructuredAnnotations()[write_props]: \TStructAnnotations {
    return shape(
      'struct' => dict[],
      'fields' => dict[
      ],
    );
  }

  public function getInstanceKey()[write_props]: string {
    return \TCompactSerializer::serialize($this);
  }

}

class TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation2_args implements \IThriftSyncStruct, \IThriftStructMetadata {
  use \ThriftSerializationTrait;

  const \ThriftStructTypes::TSpec SPEC = dict[
  ];
  const dict<string, int> FIELDMAP = dict[
  ];

  const type TConstructorShape = shape(
  );

  const int STRUCTURAL_ID = 957977401221134810;

  public function __construct()[] {
  }

  public static function withDefaultValues()[]: this {
    return new static();
  }

  public static function fromShape(self::TConstructorShape $shape)[]: this {
    return new static(
    );
  }

  public function getName()[]: string {
    return 'TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation2_args';
  }

  public function clearTerseFields()[write_props]: void {
  }

  public static function getStructMetadata()[]: \tmeta_ThriftStruct {
    return \tmeta_ThriftStruct::fromShape(
      shape(
        "name" => "module.testMethodWithServiceAnnotation2_args",
        "is_union" => false,
      )
    );
  }

  public static function getAllStructuredAnnotations()[write_props]: \TStructAnnotations {
    return shape(
      'struct' => dict[],
      'fields' => dict[
      ],
    );
  }

  public function getInstanceKey()[write_props]: string {
    return \TCompactSerializer::serialize($this);
  }

}

class TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation2_result extends \ThriftSyncStructWithoutResult implements \IThriftStructMetadata {
  use \ThriftSerializationTrait;

  const \ThriftStructTypes::TSpec SPEC = dict[
  ];
  const dict<string, int> FIELDMAP = dict[
  ];

  const type TConstructorShape = shape(
  );

  const int STRUCTURAL_ID = 957977401221134810;

  public function __construct()[] {
  }

  public static function withDefaultValues()[]: this {
    return new static();
  }

  public static function fromShape(self::TConstructorShape $shape)[]: this {
    return new static(
    );
  }

  public function getName()[]: string {
    return 'TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation2_result';
  }

  public function clearTerseFields()[write_props]: void {
  }

  public static function getStructMetadata()[]: \tmeta_ThriftStruct {
    return \tmeta_ThriftStruct::fromShape(
      shape(
        "name" => "module.TestServiceWithServiceAnnotation_testMethodWithServiceAnnotation2_result",
        "is_union" => false,
      )
    );
  }

  public static function getAllStructuredAnnotations()[write_props]: \TStructAnnotations {
    return shape(
      'struct' => dict[],
      'fields' => dict[
      ],
    );
  }

  public function getInstanceKey()[write_props]: string {
    return \TCompactSerializer::serialize($this);
  }

}

class TestServiceWithServiceAnnotationStaticMetadata implements \IThriftServiceStaticMetadata {
  public static function getServiceMetadata()[]: \tmeta_ThriftService {
    return \tmeta_ThriftService::fromShape(
      shape(
        "name" => "module.TestServiceWithServiceAnnotation",
        "functions" => vec[
          \tmeta_ThriftFunction::fromShape(
            shape(
              "name" => "testMethodWithServiceAnnotation",
              "return_type" => \tmeta_ThriftType::fromShape(
                shape(
                  "t_primitive" => \tmeta_ThriftPrimitiveType::THRIFT_I32_TYPE,
                )
              ),
            )
          ),
          \tmeta_ThriftFunction::fromShape(
            shape(
              "name" => "testMethodWithServiceAnnotation2",
              "return_type" => \tmeta_ThriftType::fromShape(
                shape(
                  "t_primitive" => \tmeta_ThriftPrimitiveType::THRIFT_VOID_TYPE,
                )
              ),
            )
          ),
        ],
      )
    );
  }

  public static function getServiceMetadataResponse()[]: \tmeta_ThriftServiceMetadataResponse {
    return \tmeta_ThriftServiceMetadataResponse::fromShape(
      shape(
        'context' => \tmeta_ThriftServiceContext::fromShape(
          shape(
            'service_info' => self::getServiceMetadata(),
            'module' => \tmeta_ThriftModuleContext::fromShape(
              shape(
                'name' => 'module',
              )
            ),
          )
        ),
        'metadata' => \tmeta_ThriftMetadata::fromShape(
          shape(
            'enums' => dict[
            ],
            'structs' => dict[
            ],
            'exceptions' => dict[
            ],
            'services' => dict[
            ],
          )
        ),
      )
    );
  }

  public static function getAllStructuredAnnotations()[write_props]: \TServiceAnnotations {
    return shape(
      'service' => dict[
        '\thrift\annotation\hack\ModuleInternal' => \thrift\annotation\hack\ModuleInternal::fromShape(
          shape(
          )
        ),
      ],
      'functions' => dict[
      ],
    );
  }
}

