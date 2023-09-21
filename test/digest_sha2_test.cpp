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
  EXPECT("47DEQpj8HBSa+/TImW+5JCeuQeRkm5NMpJWZG3hSuFU=", ==, yu::digest::sha256_base64(""));
  EXPECT("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", ==, yu::digest::sha256_hex(""));
  EXPECT("f4bb1975bf1f81f76ce824f7536c1e101a8060a632a52289d530a6f600d52c92", ==, yu::digest::sha256_hex("Hello World."));
  EXPECT("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad", ==, yu::digest::sha256_hex("abc"));

  // for (( i = 56; i < 64; ++i )); do yes X | paste -d '' -s | head -c ${i} | sha256sum | sed 's/\(.*\)  -/  EXPECT("\1", ==, yu::digest::sha256_hex(Xs('${i}')));/'; done
  EXPECT("56ad4b3f074b42bdc9c2b7b804119802f87d377335830121feb7955a23d90b54", ==, yu::digest::sha256_hex(Xs(56)));
  EXPECT("2df36666e4ea25cddd768b0cefbd8525c71f99ef24bbeaa7d1f9fb565dfe1f6e", ==, yu::digest::sha256_hex(Xs(57)));
  EXPECT("fcfcb76aa25ded82f5edcda77e973ae18a554cdef4a37b6d227f8db823d01e87", ==, yu::digest::sha256_hex(Xs(58)));
  EXPECT("9ae240a04cfff27271a2cce554c7f589c4f66b77fb129679183411bfabb3cae4", ==, yu::digest::sha256_hex(Xs(59)));
  EXPECT("9aaf85e895a79e06523885ef7e503b70bbe88205ffe8c0e237d5b0009a5965bd", ==, yu::digest::sha256_hex(Xs(60)));
  EXPECT("c19b6585717d19284e5e1a9353b4efe1d5ade24182a4b75e3df9badf73b9a654", ==, yu::digest::sha256_hex(Xs(61)));
  EXPECT("7a42222e5aa05381a7f4be0b3d78395dbefa8ebb881c7fb266f0909c125f7774", ==, yu::digest::sha256_hex(Xs(62)));
  EXPECT("f7b6b9468195f9c294529cadba2ffe8b9ea662a5cb51577e072a8a2e732cdbee", ==, yu::digest::sha256_hex(Xs(63)));
}

TEST(SHA2Test, testSHA224) {
  EXPECT("0UoCjCo6K8lHYQK7KII0xBWisB+CjqYqxbPkLw==", ==, yu::digest::sha224_base64(""));
  EXPECT("d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f", ==, yu::digest::sha224_hex(""));
  EXPECT("f871ab68ccdf47a7afb935f9f2f05365a61dee3aa6ebb7ef22be5de1", ==, yu::digest::sha224_hex("Hello World."));
  EXPECT("23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7", ==, yu::digest::sha224_hex("abc"));

  // for (( i = 56; i < 64; ++i )); do yes X | paste -d '' -s | head -c ${i} | sha224sum | sed 's/\(.*\)  -/  EXPECT("\1", ==, yu::digest::sha224_hex(Xs('${i}')));/'; done
  EXPECT("88b1314aeddbcdfd6bc16b2cc5403449ed337adaf1ffce673b64357b", ==, yu::digest::sha224_hex(Xs(56)));
  EXPECT("5066ecc81469638dd6410ab21d6606618939fdf57326edc1e77d9193", ==, yu::digest::sha224_hex(Xs(57)));
  EXPECT("2542ca3cf09b1de2e3732918e754a540d0462f24641291a85e7be72d", ==, yu::digest::sha224_hex(Xs(58)));
  EXPECT("79ec1fae4a48333cb316987df48d6287be500b71b921747678ce9903", ==, yu::digest::sha224_hex(Xs(59)));
  EXPECT("8baa32db22c7d8952530c94fa6bfd5d07599fc0f7b389be58e595b92", ==, yu::digest::sha224_hex(Xs(60)));
  EXPECT("fa546ac942f4b3534005d85d3e8c5603e54541d8291586f4ec77403b", ==, yu::digest::sha224_hex(Xs(61)));
  EXPECT("df1ad9aaf0b416bb1818e47bac5510f9254a5f8b2345389ac15ae058", ==, yu::digest::sha224_hex(Xs(62)));
  EXPECT("e01108dc02b72364c219592407855362ced5c1eb09d2f30b7d6624e2", ==, yu::digest::sha224_hex(Xs(63)));
  EXPECT("faa488c98f68199b09970a9869a2ed930551b49b59b89bc46377693b", ==, yu::digest::sha224_hex(Xs(64)));
}

TEST(SHA2Test, testSHA512) {
  EXPECT("z4PhNX7vuL3xVChQ1m2AB9Yg5AULVxXcg/SpIdNs6c5H0NE8XYXysP+DGNKHfuwvY7kxvUdBeoGl\nODJ6+SfaPg==", ==, yu::digest::sha512_base64(""));
  EXPECT("cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e", ==, yu::digest::sha512_hex(""));
  EXPECT("fee4e02329c0e1c9005d0590f4773d8e519e0cda859775ac9c83641e3a960c57e7ad461354e4860722b6e3c161e493e04f5ef07d9169ff7bdab659d6a57cc316", ==, yu::digest::sha512_hex("Hello World."));
  EXPECT("ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f", ==, yu::digest::sha512_hex("abc"));

  // for (( i = 112; i < 129; ++i )); do yes X | paste -d '' -s | head -c ${i} | sha512sum | sed 's/\(.*\)  -/  EXPECT("\1", ==, yu::digest::sha512_hex(Xs('${i}')));/'; done
  EXPECT("cd3c7dd74b8b3157b0aa132325e627a3aab8791628be09411351760fcdf3f5eb324a4e15ed699555f4cc87c5030a4d8a84cb9fb3380092fc5b71a7889f37b9d9", ==, yu::digest::sha512_hex(Xs(112)));
  EXPECT("d30df202fe5a3b38ad7bcc0c3700cb7eb5c2b3dc4547460ba1baaee8b731ecb98eb285e285b5b92682f0554cc85a23271336fba02d9b3411bc7e1f6a15e2917c", ==, yu::digest::sha512_hex(Xs(113)));
  EXPECT("3bada1fb8c0ef9f840f07abf111046a713c56ae1c89fe341fb276d5d1d3bb919a68dd9470b19e93fb9e5ffd776caa316b32dfa9bb4aa3c1f8059b06ca7c3f4e9", ==, yu::digest::sha512_hex(Xs(114)));
  EXPECT("1d266b019163a42882055502cbadedfce1394dcfb4b3863aced2fff51b7b7011beba0f0d0f16d83627a3f78ae6669541ad05c86048d6b3b1a082ada8d806f94e", ==, yu::digest::sha512_hex(Xs(115)));
  EXPECT("b342bfd1119f7facc85cfb4149e8dc30cda3bdc4baf05147819dd509f3355f3b770773a84ac30969d09706750b270035868ce118ac7f9732023abf3c5fd25dd3", ==, yu::digest::sha512_hex(Xs(116)));
  EXPECT("fe84c7bf9c380ebdb00ef4821058876dba7b4c273ee5d0e5eeea4809695e0bd7df670677cd658d533b9e90acbf3cf7d01d02de1bb25b6e83bafed00e35973f5e", ==, yu::digest::sha512_hex(Xs(117)));
  EXPECT("6781133a5458f9758f3fdee64bae1a5568f14b6f373437edea182540f87646d7724d3f8817e26bc81bc055cdc891201f66ce5dcfdf66f2f9ef41d777ab3bf7ee", ==, yu::digest::sha512_hex(Xs(118)));
  EXPECT("c425e1b7200c7dfbfd73dc36cb3dbed7dcb61eb25afb4aae6fdd009e6554bbce223482d9dca79b940803a2076e1980b85c208b98bf690e67ad4dfb21574b988b", ==, yu::digest::sha512_hex(Xs(119)));
  EXPECT("94499e292cfa281054f0ed8ec95c757e01ff9d8110ca30cc7a976307201bd93fb595e9518e5261565be3213170c5be4ef3bcdd0efdb1f4d37b39ffcef9d6218c", ==, yu::digest::sha512_hex(Xs(120)));
  EXPECT("9574e81cbff4505ca2a13be3b7aa0784efbdcd82f8aa2e624d5f29e3235522ab524a4e4541b0fd482bd29002b30aeb589e9967d4413500b5c034a87e2fdb4e4c", ==, yu::digest::sha512_hex(Xs(121)));
  EXPECT("97d53e33649c190c0cd581533a40f291fa9fcbef1ac1580a64cb6507060d05afaeaf37c499c111e6e3e6f9b2301b82f83670d9937ec29eeb49db40655ea0874f", ==, yu::digest::sha512_hex(Xs(122)));
  EXPECT("4eff34f06b9581e8cd84717ee42939ee402b7fcce7f6a4abd28b4c6feb7768f5527d48c3e1827570438118164f1ce76e13e57e313099f6cc9617db406ac85243", ==, yu::digest::sha512_hex(Xs(123)));
  EXPECT("e12dcc3bd3295fd558a7135ba0cbef3b0d3108e466325f2111c3f6eec0cf96f4f6e4530e7beffd13185c6c42c9c059c1ea570eb8ae9691ddd51c55fd132a9bd2", ==, yu::digest::sha512_hex(Xs(124)));
  EXPECT("35ad16fd8051ee66d065f6fd102a3be2cf9d3e02358512f1937df7c457db46fe9849ffc99d53a7788aa38ac91acd68463a372e2d4bd2504b598c29c9eb210273", ==, yu::digest::sha512_hex(Xs(125)));
  EXPECT("30d206a36e91c55a6df9756dd667d61a15788609ee960850dbe6f128a242772bff4990ca540568baad9f62b692b6dc41bbf0e26b3813abb674322ae4c489605c", ==, yu::digest::sha512_hex(Xs(126)));
  EXPECT("fe8dab873cae4ecd47bbf6c72692e74b726e22d2543b43a2144368ceb70d5c0223ac74e8499f432b514b9d81dbb470955122455e935801c632dc28f65391b273", ==, yu::digest::sha512_hex(Xs(127)));
  EXPECT("eab012636c4142f0daf3ce63d90e6436d4ea3ff7f0f3a4221b7aa0741679e1e6108600878eff04fda24dd562d49f114cf14a627c1ed9b9ac0bba5fc35f04e756", ==, yu::digest::sha512_hex(Xs(128)));
}

TEST(SHA2Test, testSHA384) {
  EXPECT("OLBgp1GsljhM2TJ+sbHjaiH9txEUvgdDTAzHv2P24donTt6/529l+9Ua0vFImLlb", ==, yu::digest::sha384_base64(""));
  EXPECT("38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b", ==, yu::digest::sha384_hex(""));
  EXPECT("ded020e0ea23fd2d983f7d833c44811f9e3fa96e412f84f7427250af07a5630e26366a69c44bac94fd31ec73b1b847d1", ==, yu::digest::sha384_hex("Hello World."));
  EXPECT("cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7", ==, yu::digest::sha384_hex("abc"));

  // for (( i = 112; i < 129; ++i )); do yes X | paste -d '' -s | head -c ${i} | sha384sum | sed 's/\(.*\)  -/  EXPECT("\1", ==, yu::digest::sha384_hex(Xs('${i}')));/'; done
  EXPECT("9d002185064bd12759fa6f08648bfe0347607bdd894cbad11de70ae5a033206ddcaf34291d9bf8037e9963ef22969d85", ==, yu::digest::sha384_hex(Xs(112)));
  EXPECT("a6ed36ce6b9c94709db11255ff3880f0164d3c98136e5ca0c752c97fb929e4532288854c7dd3bdbf6ff6563dbf2deeae", ==, yu::digest::sha384_hex(Xs(113)));
  EXPECT("80dd80b5c9332f28228ac0a141334045412b0f5d062be11abfb246f646f72a689fdee9e7d11f215c558f2ad950ac10f5", ==, yu::digest::sha384_hex(Xs(114)));
  EXPECT("c0591cd39542398b2cf2ddf38492d41512fba10aa99ef371fea7ffe293a705e17584915e6794db3969ca2dabbcd7712e", ==, yu::digest::sha384_hex(Xs(115)));
  EXPECT("0714df708f71b032849a41c5e7bf214c8e1c6cd7bc2663e14996698819881a1915ba8580b7c079f76203303ccdf3b856", ==, yu::digest::sha384_hex(Xs(116)));
  EXPECT("4d3413f705e661363eab57622260758ad183c3840493f61739a1e897a24b6b65c9f27bde1fc318914684bba16c516f4e", ==, yu::digest::sha384_hex(Xs(117)));
  EXPECT("9599ddc5df9b8d80b0d8ffd785fb5e2ba13e83932cc9e71442cb098b01e81aaf0570cf7b4b841a6d51f0fa3286ba54de", ==, yu::digest::sha384_hex(Xs(118)));
  EXPECT("e675bf5013224d6e7947d71b8e0c59819816b85122bd20d692608310fc204a5e4faeb967d313547c2c8f07c7664781b1", ==, yu::digest::sha384_hex(Xs(119)));
  EXPECT("b4fd92ea2aed620664633926dd7f94665f79a74b6a61f90eab9594c8cf3f0d1c2e88f1e7f6e3286821e00b5227c93c1d", ==, yu::digest::sha384_hex(Xs(120)));
  EXPECT("53047d1c7f4636157b4986396f7a2cdcc00c92c34e8fef41ebfe56415db087a50b3568b41bd05c5ce40014be6c61d78a", ==, yu::digest::sha384_hex(Xs(121)));
  EXPECT("c46b819b7b63fdfe7ed1c66689bbe25997b70acfd7b7aa34360671c790e170c45f30e05e5abe5455c106c3983b3b780d", ==, yu::digest::sha384_hex(Xs(122)));
  EXPECT("57284ee1bd1d6dc58f054a64f38ba4772cab6b2a9d97cb502b490054bd037f0885bc2b913a7de82e63d6a55c720f872b", ==, yu::digest::sha384_hex(Xs(123)));
  EXPECT("f7256e2a86a602a660b44bb7774bad06bbf339ce88f0b7c0882f3cfe927eccddee547c948ec6e950a23d5bd567c43c3c", ==, yu::digest::sha384_hex(Xs(124)));
  EXPECT("4cdf57093cdb066d6492f2d9c8de5c889129be6dab22b32ecf6416fc2ceb17e9c6f6d34cdc53e2770139d021750a85c0", ==, yu::digest::sha384_hex(Xs(125)));
  EXPECT("6bf9e04d6295469ea6f089af84190afd9efdeb5b99b254c2b5d317446116fb7d87baa1f79e04f38c79d288eca03756eb", ==, yu::digest::sha384_hex(Xs(126)));
  EXPECT("275c20b0cd66b60ee77e650eda38ad2cb4204c1010e53588bff67fef95a0fe413e74393adacced7187a1ad5735cd7b4c", ==, yu::digest::sha384_hex(Xs(127)));
  EXPECT("81e3f90c8c6c8899581b1a04070b779b04011464c68a8b4377529ba9d6b49b45bde26a20acb5ba971da47baf3bca512d", ==, yu::digest::sha384_hex(Xs(128)));
}

TEST(SHA2Test, testSHA512_224) {
  EXPECT("btDdAoBvqJ4l3gYMGdOshsq7h9ag3dBcMzuE9A==", ==, yu::digest::sha512_224_base64(""));
  EXPECT("6ed0dd02806fa89e25de060c19d3ac86cabb87d6a0ddd05c333b84f4", ==, yu::digest::sha512_224_hex(""));
  EXPECT("53a8f45fd2b7631b90d2c84b5dd223389b90ef503059f4c86fe6857d", ==, yu::digest::sha512_224_hex("Hello World."));
  EXPECT("4634270f707b6a54daae7530460842e20e37ed265ceee9a43e8924aa", ==, yu::digest::sha512_224_hex("abc"));

  // for (( i = 112; i < 129; ++i )); do yes X | paste -d '' -s | head -c ${i} | openssl sha512-224 | sed 's/.*= \(.*\)/  EXPECT("\1", ==, yu::digest::sha512_224_hex(Xs('${i}')));/'; done
  EXPECT("1d818da21eddea9ec22f5dd989d632f58c1491cc461dcb0578d11985", ==, yu::digest::sha512_224_hex(Xs(112)));
  EXPECT("8b3e91227f8cedc94a452aa6714e1272ed10605b6c18645d25412771", ==, yu::digest::sha512_224_hex(Xs(113)));
  EXPECT("0c774444508532f0536a300679f36f5f124944c3835d6bca64d12fa3", ==, yu::digest::sha512_224_hex(Xs(114)));
  EXPECT("f1d948a193b72bc6dd8a96b2cc650622f1d8307341c7dc3b4eaa840f", ==, yu::digest::sha512_224_hex(Xs(115)));
  EXPECT("5f2f212271e0728d01a445f72b66419a8d8fc9c8cc5d2f579d62e7d5", ==, yu::digest::sha512_224_hex(Xs(116)));
  EXPECT("c28114189d96e707993145bbe844ea5eb7fb2cbe1eb8d4de8daef652", ==, yu::digest::sha512_224_hex(Xs(117)));
  EXPECT("26f2b97d02bb5739935e494e935d2f91ed6be5794e5b937f21cd7a80", ==, yu::digest::sha512_224_hex(Xs(118)));
  EXPECT("77d2f7b15997176a2224dfe689bf5770ed8836f0da329f5be9b8b1d0", ==, yu::digest::sha512_224_hex(Xs(119)));
  EXPECT("02a099478195e811b3c92d90f09e92bae770a5c623dd67d981a17921", ==, yu::digest::sha512_224_hex(Xs(120)));
  EXPECT("914e1a132ff59da516faeec1c2caddc421a3a0af80d0f8646ffdd2c9", ==, yu::digest::sha512_224_hex(Xs(121)));
  EXPECT("61c23dcc39b9d8cf65c86f8f37d25ec5a11b8fdf768c9331be00daed", ==, yu::digest::sha512_224_hex(Xs(122)));
  EXPECT("8c384ef986b1cc3ad01738fa7bace70fb0fa78f10a38ed8169db19a1", ==, yu::digest::sha512_224_hex(Xs(123)));
  EXPECT("ddd7d62704560eba642e8252f344b491ee8d433b94333d2230f504cc", ==, yu::digest::sha512_224_hex(Xs(124)));
  EXPECT("f4b06b5192c21537c8f69c6bee98cf8a93e127933b7c2273fefec84b", ==, yu::digest::sha512_224_hex(Xs(125)));
  EXPECT("81397256b2cc72ccb8584cc7b3a7848a5a7b23908a3b40089fa56818", ==, yu::digest::sha512_224_hex(Xs(126)));
  EXPECT("a019d957512d57fddebd3d879ba07384438913cc0976b8bee052e8cd", ==, yu::digest::sha512_224_hex(Xs(127)));
  EXPECT("5a60066cddb26a204817564f58a23b81a2142e9fabef769a7b779db8", ==, yu::digest::sha512_224_hex(Xs(128)));
}

TEST(SHA2Test, testSHA512_256) {
  EXPECT("xnK40e9W7Sirh8NiLFEUBpvdOte4+XN0mNDAHs7wlno=", ==, yu::digest::sha512_256_base64(""));
  EXPECT("c672b8d1ef56ed28ab87c3622c5114069bdd3ad7b8f9737498d0c01ecef0967a", ==, yu::digest::sha512_256_hex(""));
  EXPECT("cc296ed308cbe384e0de66c8580b3373ac2ae88dd53a9bd8542df1431e87f01d", ==, yu::digest::sha512_256_hex("Hello World."));
  EXPECT("53048e2681941ef99b2e29b76b4c7dabe4c2d0c634fc6d46e0e2f13107e7af23", ==, yu::digest::sha512_256_hex("abc"));

  // for (( i = 112; i < 129; ++i )); do yes X | paste -d '' -s | head -c ${i} | openssl sha512-256 | sed 's/.*= \(.*\)/  EXPECT("\1", ==, yu::digest::sha512_256_hex(Xs('${i}')));/'; done
  EXPECT("b3c8549d4cebcd8229ff2cdbc26e1460b3fec8727fd0ccefaaa8bf50c9c68d96", ==, yu::digest::sha512_256_hex(Xs(112)));
  EXPECT("093c34acf07fdc107307064a407707f5d5ac18bb3e051c6f1ccf5584aa79533f", ==, yu::digest::sha512_256_hex(Xs(113)));
  EXPECT("1988401a9acc91237313e8bab0307d646245557ef0e3cf5710b0b5259b819034", ==, yu::digest::sha512_256_hex(Xs(114)));
  EXPECT("0797d9c472c1d1cad16a27cdd8a93f1aaf51ff9102eccefea0191693d0dbce6b", ==, yu::digest::sha512_256_hex(Xs(115)));
  EXPECT("8e7c2dce8900b8dce29545329e42de140eb762054665441edf6bf98dfddeafc7", ==, yu::digest::sha512_256_hex(Xs(116)));
  EXPECT("be29a08344e1ef221dbef179563970f6f8b112ca5f59614a9dd09eff69a0408d", ==, yu::digest::sha512_256_hex(Xs(117)));
  EXPECT("b5fdf067694575ff60baa9870f445d9986759785e51aaac64fcbbba6b6e347c7", ==, yu::digest::sha512_256_hex(Xs(118)));
  EXPECT("30dd15ee4eb6122237ea49d36e1bf5973652b1398022465b38141b2e84cf188c", ==, yu::digest::sha512_256_hex(Xs(119)));
  EXPECT("db768c22d0d361121c607d6cb2105b52feb3729e1478d8732bab96ec30011d5f", ==, yu::digest::sha512_256_hex(Xs(120)));
  EXPECT("c44d86bd7e1c9dcfa25a12e53f81100bff59232cc122bb59a10f3fdf07ee492b", ==, yu::digest::sha512_256_hex(Xs(121)));
  EXPECT("b458721cccb44a529771fe57aebddcbd1d44e32a52443e16f33fa37b7495da55", ==, yu::digest::sha512_256_hex(Xs(122)));
  EXPECT("ebbe22add7262a597fe14ae0ff2610882d53cd8d06b5f44b4924c9fbbfd05401", ==, yu::digest::sha512_256_hex(Xs(123)));
  EXPECT("7f025399ac931a910385f91f7eea8d055413296de3e45ac97bb78080ec1a2d69", ==, yu::digest::sha512_256_hex(Xs(124)));
  EXPECT("30246027d0b0511b1c6bf899715688a4b0e1bccafefd813b4df898550f81d65a", ==, yu::digest::sha512_256_hex(Xs(125)));
  EXPECT("97b83cd15680c709a88dfd1258b34717d5fb9126f462f363967aaae73bbefc35", ==, yu::digest::sha512_256_hex(Xs(126)));
  EXPECT("d417d20215cdc65351afec51e889fe9bcd5db93263c9afa2833bb90ab9487f6d", ==, yu::digest::sha512_256_hex(Xs(127)));
  EXPECT("d16103e8a4e06a4b8cdaa552bf0a0f9297d68f89e5fcb092ab484ddf2db4519d", ==, yu::digest::sha512_256_hex(Xs(128)));
}
