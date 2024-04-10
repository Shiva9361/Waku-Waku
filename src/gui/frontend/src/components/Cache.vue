<template>
  <div>
    <div v-for="(value2, key2) in cache" :key="key2">
      <div class="c_key">{{ key2 }}</div>
      <div class="c_value">{{ value2 }}</div>
    </div>
  </div>
</template>

<script>
export default {
  name: "cacheComponent",
  props: {
    cache: Object,
    split_cache: Object,
  },
};
</script>

<style>
.c_key {
  margin: 1px 1px 0px 0px;
  border-style: solid;
  display: inline-block;
  width: 70px;
}
.c_value {
  border-style: solid;
  display: inline-block;
  width: 300px;
}
</style>
