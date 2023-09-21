#include "yu/digest/sha2.hpp"

#include "yu/test.hpp"

class SHA2Test : public yu::Test {
 protected:
  std::string Xs(int n) { return std::string(n, 'X'); }
};

TEST(SHA2Test, testRotR32) {
  EXPECT(0x81234567, ==, yu::digest::rotr<uint32_t>(0x12345678UL, 4));
  EXPECT(0x78123456, ==, yu::digest::rotr<uint32_t>(0x12345678UL, 8));
}

TEST(SHA2Test, testRotR64) {
  EXPECT(0x0123456789ABCDEFULL, ==, yu::digest::rotr<uint64_t>(0x123456789ABCDEF0ULL, 4));
  EXPECT(0xF0123456789ABCDEULL, ==, yu::digest::rotr<uint64_t>(0x123456789ABCDEF0ULL, 8));
}

TEST(SHA2Test, testSHA256) {
  EXPECT("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", ==, yu::digest::sha256(""));
  EXPECT("f4bb1975bf1f81f76ce824f7536c1e101a8060a632a52289d530a6f600d52c92", ==, yu::digest::sha256("Hello World."));
  EXPECT("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad", ==, yu::digest::sha256("abc"));

  // for (( i = 56; i < 64; ++i )); do yes X | paste -d '' -s | head -c ${i} | sha256sum | sed 's/\(.*\)  -/  EXPECT("\1", ==, yu::digest::sha256(Xs('${i}')));/'; done
  EXPECT("56ad4b3f074b42bdc9c2b7b804119802f87d377335830121feb7955a23d90b54", ==, yu::digest::sha256(Xs(56)));
  EXPECT("2df36666e4ea25cddd768b0cefbd8525c71f99ef24bbeaa7d1f9fb565dfe1f6e", ==, yu::digest::sha256(Xs(57)));
  EXPECT("fcfcb76aa25ded82f5edcda77e973ae18a554cdef4a37b6d227f8db823d01e87", ==, yu::digest::sha256(Xs(58)));
  EXPECT("9ae240a04cfff27271a2cce554c7f589c4f66b77fb129679183411bfabb3cae4", ==, yu::digest::sha256(Xs(59)));
  EXPECT("9aaf85e895a79e06523885ef7e503b70bbe88205ffe8c0e237d5b0009a5965bd", ==, yu::digest::sha256(Xs(60)));
  EXPECT("c19b6585717d19284e5e1a9353b4efe1d5ade24182a4b75e3df9badf73b9a654", ==, yu::digest::sha256(Xs(61)));
  EXPECT("7a42222e5aa05381a7f4be0b3d78395dbefa8ebb881c7fb266f0909c125f7774", ==, yu::digest::sha256(Xs(62)));
  EXPECT("f7b6b9468195f9c294529cadba2ffe8b9ea662a5cb51577e072a8a2e732cdbee", ==, yu::digest::sha256(Xs(63)));
}

TEST(SHA2Test, testSHA224) {
  EXPECT("d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f", ==, yu::digest::sha224(""));
  EXPECT("f871ab68ccdf47a7afb935f9f2f05365a61dee3aa6ebb7ef22be5de1", ==, yu::digest::sha224("Hello World."));
  EXPECT("23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7", ==, yu::digest::sha224("abc"));

  // for (( i = 56; i < 64; ++i )); do yes X | paste -d '' -s | head -c ${i} | sha224sum | sed 's/\(.*\)  -/  EXPECT("\1", ==, yu::digest::sha224(Xs('${i}')));/'; done
  EXPECT("88b1314aeddbcdfd6bc16b2cc5403449ed337adaf1ffce673b64357b", ==, yu::digest::sha224(Xs(56)));
  EXPECT("5066ecc81469638dd6410ab21d6606618939fdf57326edc1e77d9193", ==, yu::digest::sha224(Xs(57)));
  EXPECT("2542ca3cf09b1de2e3732918e754a540d0462f24641291a85e7be72d", ==, yu::digest::sha224(Xs(58)));
  EXPECT("79ec1fae4a48333cb316987df48d6287be500b71b921747678ce9903", ==, yu::digest::sha224(Xs(59)));
  EXPECT("8baa32db22c7d8952530c94fa6bfd5d07599fc0f7b389be58e595b92", ==, yu::digest::sha224(Xs(60)));
  EXPECT("fa546ac942f4b3534005d85d3e8c5603e54541d8291586f4ec77403b", ==, yu::digest::sha224(Xs(61)));
  EXPECT("df1ad9aaf0b416bb1818e47bac5510f9254a5f8b2345389ac15ae058", ==, yu::digest::sha224(Xs(62)));
  EXPECT("e01108dc02b72364c219592407855362ced5c1eb09d2f30b7d6624e2", ==, yu::digest::sha224(Xs(63)));
  EXPECT("faa488c98f68199b09970a9869a2ed930551b49b59b89bc46377693b", ==, yu::digest::sha224(Xs(64)));
}

TEST(SHA2Test, testSHA512) {
  EXPECT("cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e", ==, yu::digest::sha512(""));
  EXPECT("fee4e02329c0e1c9005d0590f4773d8e519e0cda859775ac9c83641e3a960c57e7ad461354e4860722b6e3c161e493e04f5ef07d9169ff7bdab659d6a57cc316", ==, yu::digest::sha512("Hello World."));
  EXPECT("ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f", ==, yu::digest::sha512("abc"));

  // for (( i = 112; i < 129; ++i )); do yes X | paste -d '' -s | head -c ${i} | sha512sum | sed 's/\(.*\)  -/  EXPECT("\1", ==, yu::digest::sha512(Xs('${i}')));/'; done
  EXPECT("cd3c7dd74b8b3157b0aa132325e627a3aab8791628be09411351760fcdf3f5eb324a4e15ed699555f4cc87c5030a4d8a84cb9fb3380092fc5b71a7889f37b9d9", ==, yu::digest::sha512(Xs(112)));
  EXPECT("d30df202fe5a3b38ad7bcc0c3700cb7eb5c2b3dc4547460ba1baaee8b731ecb98eb285e285b5b92682f0554cc85a23271336fba02d9b3411bc7e1f6a15e2917c", ==, yu::digest::sha512(Xs(113)));
  EXPECT("3bada1fb8c0ef9f840f07abf111046a713c56ae1c89fe341fb276d5d1d3bb919a68dd9470b19e93fb9e5ffd776caa316b32dfa9bb4aa3c1f8059b06ca7c3f4e9", ==, yu::digest::sha512(Xs(114)));
  EXPECT("1d266b019163a42882055502cbadedfce1394dcfb4b3863aced2fff51b7b7011beba0f0d0f16d83627a3f78ae6669541ad05c86048d6b3b1a082ada8d806f94e", ==, yu::digest::sha512(Xs(115)));
  EXPECT("b342bfd1119f7facc85cfb4149e8dc30cda3bdc4baf05147819dd509f3355f3b770773a84ac30969d09706750b270035868ce118ac7f9732023abf3c5fd25dd3", ==, yu::digest::sha512(Xs(116)));
  EXPECT("fe84c7bf9c380ebdb00ef4821058876dba7b4c273ee5d0e5eeea4809695e0bd7df670677cd658d533b9e90acbf3cf7d01d02de1bb25b6e83bafed00e35973f5e", ==, yu::digest::sha512(Xs(117)));
  EXPECT("6781133a5458f9758f3fdee64bae1a5568f14b6f373437edea182540f87646d7724d3f8817e26bc81bc055cdc891201f66ce5dcfdf66f2f9ef41d777ab3bf7ee", ==, yu::digest::sha512(Xs(118)));
  EXPECT("c425e1b7200c7dfbfd73dc36cb3dbed7dcb61eb25afb4aae6fdd009e6554bbce223482d9dca79b940803a2076e1980b85c208b98bf690e67ad4dfb21574b988b", ==, yu::digest::sha512(Xs(119)));
  EXPECT("94499e292cfa281054f0ed8ec95c757e01ff9d8110ca30cc7a976307201bd93fb595e9518e5261565be3213170c5be4ef3bcdd0efdb1f4d37b39ffcef9d6218c", ==, yu::digest::sha512(Xs(120)));
  EXPECT("9574e81cbff4505ca2a13be3b7aa0784efbdcd82f8aa2e624d5f29e3235522ab524a4e4541b0fd482bd29002b30aeb589e9967d4413500b5c034a87e2fdb4e4c", ==, yu::digest::sha512(Xs(121)));
  EXPECT("97d53e33649c190c0cd581533a40f291fa9fcbef1ac1580a64cb6507060d05afaeaf37c499c111e6e3e6f9b2301b82f83670d9937ec29eeb49db40655ea0874f", ==, yu::digest::sha512(Xs(122)));
  EXPECT("4eff34f06b9581e8cd84717ee42939ee402b7fcce7f6a4abd28b4c6feb7768f5527d48c3e1827570438118164f1ce76e13e57e313099f6cc9617db406ac85243", ==, yu::digest::sha512(Xs(123)));
  EXPECT("e12dcc3bd3295fd558a7135ba0cbef3b0d3108e466325f2111c3f6eec0cf96f4f6e4530e7beffd13185c6c42c9c059c1ea570eb8ae9691ddd51c55fd132a9bd2", ==, yu::digest::sha512(Xs(124)));
  EXPECT("35ad16fd8051ee66d065f6fd102a3be2cf9d3e02358512f1937df7c457db46fe9849ffc99d53a7788aa38ac91acd68463a372e2d4bd2504b598c29c9eb210273", ==, yu::digest::sha512(Xs(125)));
  EXPECT("30d206a36e91c55a6df9756dd667d61a15788609ee960850dbe6f128a242772bff4990ca540568baad9f62b692b6dc41bbf0e26b3813abb674322ae4c489605c", ==, yu::digest::sha512(Xs(126)));
  EXPECT("fe8dab873cae4ecd47bbf6c72692e74b726e22d2543b43a2144368ceb70d5c0223ac74e8499f432b514b9d81dbb470955122455e935801c632dc28f65391b273", ==, yu::digest::sha512(Xs(127)));
  EXPECT("eab012636c4142f0daf3ce63d90e6436d4ea3ff7f0f3a4221b7aa0741679e1e6108600878eff04fda24dd562d49f114cf14a627c1ed9b9ac0bba5fc35f04e756", ==, yu::digest::sha512(Xs(128)));
}

TEST(SHA2Test, testSHA384) {
  EXPECT("38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b", ==, yu::digest::sha384(""));
  EXPECT("ded020e0ea23fd2d983f7d833c44811f9e3fa96e412f84f7427250af07a5630e26366a69c44bac94fd31ec73b1b847d1", ==, yu::digest::sha384("Hello World."));
  EXPECT("cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7", ==, yu::digest::sha384("abc"));

  // for (( i = 112; i < 129; ++i )); do yes X | paste -d '' -s | head -c ${i} | sha384sum | sed 's/\(.*\)  -/  EXPECT("\1", ==, yu::digest::sha384(Xs('${i}')));/'; done
  EXPECT("9d002185064bd12759fa6f08648bfe0347607bdd894cbad11de70ae5a033206ddcaf34291d9bf8037e9963ef22969d85", ==, yu::digest::sha384(Xs(112)));
  EXPECT("a6ed36ce6b9c94709db11255ff3880f0164d3c98136e5ca0c752c97fb929e4532288854c7dd3bdbf6ff6563dbf2deeae", ==, yu::digest::sha384(Xs(113)));
  EXPECT("80dd80b5c9332f28228ac0a141334045412b0f5d062be11abfb246f646f72a689fdee9e7d11f215c558f2ad950ac10f5", ==, yu::digest::sha384(Xs(114)));
  EXPECT("c0591cd39542398b2cf2ddf38492d41512fba10aa99ef371fea7ffe293a705e17584915e6794db3969ca2dabbcd7712e", ==, yu::digest::sha384(Xs(115)));
  EXPECT("0714df708f71b032849a41c5e7bf214c8e1c6cd7bc2663e14996698819881a1915ba8580b7c079f76203303ccdf3b856", ==, yu::digest::sha384(Xs(116)));
  EXPECT("4d3413f705e661363eab57622260758ad183c3840493f61739a1e897a24b6b65c9f27bde1fc318914684bba16c516f4e", ==, yu::digest::sha384(Xs(117)));
  EXPECT("9599ddc5df9b8d80b0d8ffd785fb5e2ba13e83932cc9e71442cb098b01e81aaf0570cf7b4b841a6d51f0fa3286ba54de", ==, yu::digest::sha384(Xs(118)));
  EXPECT("e675bf5013224d6e7947d71b8e0c59819816b85122bd20d692608310fc204a5e4faeb967d313547c2c8f07c7664781b1", ==, yu::digest::sha384(Xs(119)));
  EXPECT("b4fd92ea2aed620664633926dd7f94665f79a74b6a61f90eab9594c8cf3f0d1c2e88f1e7f6e3286821e00b5227c93c1d", ==, yu::digest::sha384(Xs(120)));
  EXPECT("53047d1c7f4636157b4986396f7a2cdcc00c92c34e8fef41ebfe56415db087a50b3568b41bd05c5ce40014be6c61d78a", ==, yu::digest::sha384(Xs(121)));
  EXPECT("c46b819b7b63fdfe7ed1c66689bbe25997b70acfd7b7aa34360671c790e170c45f30e05e5abe5455c106c3983b3b780d", ==, yu::digest::sha384(Xs(122)));
  EXPECT("57284ee1bd1d6dc58f054a64f38ba4772cab6b2a9d97cb502b490054bd037f0885bc2b913a7de82e63d6a55c720f872b", ==, yu::digest::sha384(Xs(123)));
  EXPECT("f7256e2a86a602a660b44bb7774bad06bbf339ce88f0b7c0882f3cfe927eccddee547c948ec6e950a23d5bd567c43c3c", ==, yu::digest::sha384(Xs(124)));
  EXPECT("4cdf57093cdb066d6492f2d9c8de5c889129be6dab22b32ecf6416fc2ceb17e9c6f6d34cdc53e2770139d021750a85c0", ==, yu::digest::sha384(Xs(125)));
  EXPECT("6bf9e04d6295469ea6f089af84190afd9efdeb5b99b254c2b5d317446116fb7d87baa1f79e04f38c79d288eca03756eb", ==, yu::digest::sha384(Xs(126)));
  EXPECT("275c20b0cd66b60ee77e650eda38ad2cb4204c1010e53588bff67fef95a0fe413e74393adacced7187a1ad5735cd7b4c", ==, yu::digest::sha384(Xs(127)));
  EXPECT("81e3f90c8c6c8899581b1a04070b779b04011464c68a8b4377529ba9d6b49b45bde26a20acb5ba971da47baf3bca512d", ==, yu::digest::sha384(Xs(128)));
}
